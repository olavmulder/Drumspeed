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

def CalculateTimeError(id, minute, second):
    i = 0
    amountInvalidTimeFrame = 0
    amountOneSecOff = 0
    lastIndex = 0
    
    for index in second:
        '''
        goed als:
            time == 30 && minuut == 0 ( 22 && 52)
            time == -30 && minuut == -1 (52 && 22)

        '''
        time = index - lastIndex
        if(i > 0):
            minuteValue = minute[i] - minute[i-1]
            if (minute[i] == 0 and minute[i-1] == 59):
                minuteValue = 1
            if(not (time == 30 and minuteValue == 0) and not(time == -30 and minuteValue == 1)):

                amountInvalidTimeFrame += 1
                print( "id", id[i], ": ", index, "- ", lastIndex, " = ", time) 
          
        lastIndex = index
        i = i + 1
    
    return amountInvalidTimeFrame
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
                listRPMErrorId.append(id[x+1])
                listRPMErrorId.append(id[x])
                listRPMErrorId.append(id[x-1])
                x+=1
            if(x != amountRecords):
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

def CalculateMaxMinAvg(connection, queryMin, queryMax, queryAvg):
    result = GetArray(queryMax, connection)
    max =  [row[0] for row in result]
    max = max[0]
    result = GetArray(queryMin, connection)
    min = [row[0] for row in result]
    min = min[0]
    result = GetArray(queryAvg, connection)
    sum = 0
    for x in result:
        sum += x[0]
    avg = sum/len(result)
    return min,max,avg


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

    #min, max, gem values
    queryMin = "SELECT MIN(RPM) FROM data"
    queryMax = "SELECT MAX(RPM) FROM data"
    queryAvg = "SELECT RPM FROM data"
    result = CalculateMaxMinAvg(connection, queryMin, queryMax, queryAvg)
    minValue = result[0]
    maxValue = result[1]
    avgValue = result[2]
    
    #select data where measementtime isn't null
    query = 'SELECT id, measurementTime FROM data WHERE measurementTime IS NOT NULL'
    result = GetArray(query, connection)

    #get wrong measurement times
    seconde = [row[1].second for row in result]
    minute =  [row[1].minute for row in result]
    id = [row[0] for row in result]
    returnValues = CalculateTimeError(id, minute, seconde)
    amountInvalidTimeFrame = returnValues
        
    

    #calculate diff error between values
    amountDiffError = CalculateMeasurementError(connection)
    
    #reference between speed belt & speed drum
    #get first and last measurement time
    
    #startDate = datetime.fromisoformat('2021-12-09 11:50:00')
    #stopDate = datetime.fromisoformat('2021-12-09 12:20:00')
    #by hand, get all the measured beltspeed values form the dashboard
    #beltSpeed = InsertSpeedFromDashboard(startDate, stopDate)
    #returnData  = CompareDrumSpeedWithBeltSpeed(startDate, stopDate, connection, beltSpeed)
    #amountDiffSpeedError = returnData[0]
    #totalRowCompare = returnData[1]

    '''
    done: amount record, null values, above limit, under null, difference error betwee values, timeframe errors, compare drumspeed with beltspeed
    '''
    print("\ngeneral data:\n")
    print("amount records: ",           amountRecords)

    print("max:", maxValue, " min: ", minValue, " avg: ",avgValue)

    print("amount null values: ",       amountNullValues)
    print("amount above limit(1200): ", amountaboveLimit)
    print("amount under null: ",        amountUnderNull)    

    print("amount diff error:",         amountDiffError)
    #print("amount diff Speed error:",   amountDiffSpeedError)
    print("amount invalid time frame: ", amountInvalidTimeFrame)
    '''
    percentage calculation
    '''
    percentage = float((amountInvalidTimeFrame/amountRecords)*100)
    print("time frame fault in percentage", percentage)

    percentage = float(((amountUnderNull+amountNullValues+amountaboveLimit)/amountRecords)*100)
    print("invalid values in percentage", percentage)

    percentage = float((amountDiffError/amountRecords)*100)
    print("Difference error in percentage:", percentage)
 
    #percentage = float((amountDiffSpeedError/totalRowCompare)*100)
    #print("Difference error between speed of belt & drum in percentage:", percentage)

    exit(0)