from datetime import datetime
import sys
sys.path.insert(0,'.')
import mysqlFunctions
'''
get data below zero, probably nothing!!
get rpm above 1200 rpm
get difference between each measurement and the next one.
--later--get drum speed & belt speed on same time
'''
def GetArray(query, connection):

    cursor = connection.cursor()
    cursor.execute(query)
    records = cursor.fetchall()
    return records
def GetAmountNullValues(invalidValues):
    i=0
    amountNullValues = 0
    for index in invalidValues:
        if index == 0:
            amountNullValues += 1
        i+=1
    return amountNullValues

def CalculateTimeError(id, second):
    i = 0
    amountInvalidTimeFrame = 0
    amountOneSecOff = 0
    lastIndex = 0
    for index in second:
        time = index - lastIndex
        
        if (time != 30) and (time != -30) and i != 0:
            amountInvalidTimeFrame += 1
            if(time == -29 or time == -31 or time == 31 or time == 29):
                amountOneSecOff += 1
            else:
                print( "id", id[i], ": ", index, "- ", lastIndex, " = ", time) 
        lastIndex = index
        i = i + 1
    
    return amountInvalidTimeFrame,amountOneSecOff
def CalculateMeasurementError(connection):
    #get id's where rpm is 1 of more different from last id-> error = 0.1 %
    
    #query = "SELECT id, rpm FROM data WHERE measurementTime < '2021-12-2 14:49:12'"
    query = "SELECT id, rpm FROM data"
    data = GetArray(query, connection)
    if (len(data) != 0):
        id = [row[0] for row in data]
        rpm = [row[1] for row in data]
        oldData = 0
        amountDiffError = 0
        listRPMErrorId= []
        amountRecords = len(id)
        for x in range(amountRecords):
            if x == 0:
                oldData = rpm[x]
            diff = rpm[x] - oldData
            if not(diff > -1 and diff < 1):
                amountDiffError += 1
                listRPMErrorId.append(id[x])
            oldData = rpm[x]
        sql_list = str(tuple([listRPMErrorId])).replace('],','')
        if len(listRPMErrorId) != 0:
            sql_list = str(sql_list).replace('[','')
            query = """SELECT * FROM data WHERE id IN {}""".format(sql_list)
            data = GetArray(query,connection)
            print("detected differences here:")
            stringList =["id", "speed", "time", "rpm", "amount", "measurementTime"]
            print ("{: >10} {: >10} {} {: >25} {: >10} {}".format(*stringList))
            for a in data:
                print("{: >10} {: >10} {} {: >10} {: >10} {}".format(*a))
        return amountDiffError

def InsertSpeedFromDashboard(startTime, stopTime):
    interval = 10
    minutes = (stopTime - startTime).seconds
    amount = int(minutes/60/interval)
    speedRef = list()
    for x in range(amount):
        print(x,': ')
        value = float(input())
        speedRef.append(value)
    return speedRef


def CompareDrumSpeedWithBeltSpeed(startTime, stopTime, connection, beltspeed):
    #get speedValues between startTime & stopTime, start is earlier
    start = startTime.strftime("%Y-%m-%d %H:%M:%S")
    stop = stopTime.strftime("%Y-%m-%d %H:%M:%S")
    query = "SELECT id,speed, measurementTime FROM data WHERE measurementTime BETWEEN '{}' and '{}'".format(start, stop)

    result = GetArray(query,connection)
    speed = [row[1] for row in result]
    measurementTime = [row[2] for row in result]
    #calculate avg drumspeed from 10 minutes 
    totalSpeed = 0
    avgSpeed = list()
    for x in range(len(result)):
        totalSpeed = totalSpeed + speed[x]
        if(x%20 == 0 and x != 0):
            avgSpeed.append(totalSpeed/20)
    diffSpeed = list()
    amountDiffSpeedError =0
    #print difference between beltspeed & drumspeed
    for x in range(len(avgSpeed)):
        print(avgSpeed[x],'-',beltSpeed[x], '=', avgSpeed[x]-beltSpeed[x])
        diff = avgSpeed[x]-beltspeed[x]
        diffSpeed.append(diff)
        if(not(diff > -1 and diff < 1)):
            amountDiffSpeedError +=1
    
    return amountDiffSpeedError, len(avgSpeed)

        
if __name__ == '__main__':   
    #make connection with db
    connection = mysqlFunctions.MysqlConnect('olav', 'Ammeraal123!', '127.0.0.1',"testTable")
    
    #get amount rows in table
    query = ("SELECT COUNT(*) from data")
    amountRecords = mysqlFunctions.MysqlQueryReturn(query, connection)[0]               
    
    #get all id with values lower than 0
    query = "SELECT id, RPM FROM data WHERE RPM <= 0"
    data = GetArray(query,connection)
    nullValues = [row[1] for row in data]

    amountNullValues = GetAmountNullValues(nullValues)     #amount null valus in invalidValues array
    amountUnderNull = len(nullValues) - amountNullValues

    #get all id with values higher than 1200
    query = "SELECT id, RPM FROM data WHERE RPM >=1200"
    data = GetArray(query, connection)
    aboveLimit = [row[1] for row in data]
    amountaboveLimit = len(aboveLimit)

    #select data where measementtime isn't null
    query = 'SELECT id, measurementTime FROM data WHERE measurementTime IS NOT NULL'
    result = GetArray(query, connection)

    #get wrong measurement times
    seconde = [row[1].second for row in result]
    id = [row[0] for row in result]
    returnValues = CalculateTimeError(id, seconde)
    amountInvalidTimeFrame = returnValues[0]
    amountOneSecOff = returnValues[1]
    
    

    #calculate diff error between values
    amountDiffError = CalculateMeasurementError(connection)
    
    #reference between speed belt & speed drum
    #get first and last measurement time
    
    startDate = datetime.fromisoformat('2021-12-09 11:50:00')
    stopDate = datetime.fromisoformat('2021-12-09 12:20:00')
    #by hand, get all the measured beltspeed values form the dashboard
    beltSpeed = InsertSpeedFromDashboard(startDate, stopDate)
    returnData  = CompareDrumSpeedWithBeltSpeed(startDate, stopDate, connection, beltSpeed)
    amountDiffSpeedError = returnData[0]
    totalRowCompare = returnData[1]

    '''
    done: amount record, null values, above limit, under null, difference error betwee values, timeframe errors, compare drumspeed with beltspeed
    '''
    print("\ngeneral data:\n")
    print("amount records: ",           amountRecords)
    print("amount null values: ",       amountNullValues)
    print("amount above limit(1200): ", amountaboveLimit)
    print("amount under null: ",        amountUnderNull)    

    print("amount diff error:",         amountDiffError)
    print("amount diff Speed error:",   amountDiffSpeedError)
    print("amount invalid time frame: ", amountInvalidTimeFrame, "with ",amountOneSecOff, "one sec off")
    '''
    percentage calculation
    '''
    percentage = float((amountInvalidTimeFrame/amountRecords)*100)
    print("time frame fault in percentage", percentage)

    percentage = float(((amountUnderNull+amountNullValues+amountaboveLimit)/amountRecords)*100)
    print("invalid values in percentage", percentage)

    percentage = float((amountDiffError/amountRecords)*100)
    print("Difference error in percentage:", percentage)
 
    percentage = float((amountDiffSpeedError/totalRowCompare)*100)
    print("Difference error between speed of belt & drum in percentage:", percentage)
    
    exit(0)