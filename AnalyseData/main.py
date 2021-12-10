import sys, datetime
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
    return cursor.fetchall()
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

def CompareDrumSpeedWithBeltSpeed(amountRecords, connection):
    '''for x time get last records'''
    lastID = mysqlFunctions.MysqlQueryReturn("Select MAX(id) from data", connection)
    firstID = lastID[0]-amountRecords
    lastID = lastID[0]
    query = f"SELECT speed, time FROM data WHERE id BETWEEN {firstID} AND {lastID} ORDER BY id DESC"
    '''take average of ten minutes'''
    
    returnArray = GetArray(query, connection)
    speed = [row[0] for row in returnArray]
    time  = [row[1] for row in returnArray]
    speedTotaal = 0
    lenSpeed = len(speed)
    avgSpeed = list()
    timeArray = list()
    for x in range(lenSpeed):
        speedTotaal += speed[x]
        if x%20 == 0 and x != 0:
            avgSpeed.append(float(speedTotaal/20))
            timeArray.append(time[x])
            speedTotaal = 0
    print("\navg speed over 10 minutes from last to first:")
    i=0
    for a in avgSpeed:
        print("{:.2f}".format(a), timeArray[i])
        i= i+1
    '''compare that difference'''

if __name__ == '__main__':   
    connection = mysqlFunctions.MysqlConnect('olav', 'Ammeraal123!', '127.0.0.1',"testTable")
    
    query = ("SELECT COUNT(*) from data")
    amountRecords = mysqlFunctions.MysqlQueryReturn(query, connection)[0]
    

    query = "SELECT id FROM data WHERE rpm <= 0.0 OR rpm > 1200 "
    data = GetArray(query, connection)
    
    invalidValuesID = [row[0] for row in data]                #array of id's from invalidValues 
    
    query = "SELECT id, RPM FROM data WHERE RPM <= 0"
    data = GetArray(query,connection)
    nullValues = [row[1] for row in data]

    #invalidValues   = [row[1] for row in data]                #array of values form invalidValues
    amountNullValues = GetAmountNullValues(nullValues)     #amount null valus in invalidValues array
    amountUnderNull = len(nullValues) - amountNullValues

    query = "SELECT id, RPM FROM data WHERE RPM >=1200"
    data = GetArray(query, connection)
    aboveLimit = [row[1] for row in data]
    amountaboveLimit = len(aboveLimit)

    query = 'SELECT id, measurementTime FROM data WHERE measurementTime IS NOT NULL'
    result = GetArray(query, connection)

    seconde = [row[1].second for row in result]
    id = [row[0] for row in result]
    returnValues = CalculateTimeError(id, seconde)
    amountInvalidTimeFrame = returnValues[0]
    amountOneSecOff = returnValues[1]
    
    '''
    get id's where rpm is 1 of more different from last id-> error = 0.1 %
    '''
    query = "SELECT id, rpm FROM data WHERE measurementTime < '2021-12-2 14:49:12'"
    data = GetArray(query, connection)
    id = [row[0] for row in data]
    rpm = [row[1] for row in data]
    oldData = 0
    amountDiffError = 0
    listRPMErrorId= []
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

    #get first and last measurement time, find maybe stopt program
    CompareDrumSpeedWithBeltSpeed(100, connection)
    print("\ngeneral data:\n")
    print("amount records: ",           amountRecords)
    print("amount null values: ",       amountNullValues)
    print("amount above limit(1200): ", amountaboveLimit)
    print("amount under null: ",        amountUnderNull)    

    print("amount diff error:",         amountDiffError)

    print("amount invalid time frame: ", amountInvalidTimeFrame, "with ",amountOneSecOff, "one sec off")
    '''
    percentage calculation
    '''
    percentage = float((amountInvalidTimeFrame/amountRecords)*100)
    print("time frame fault in percentage", percentage)

    percentage = float(((amountUnderNull+amountNullValues+amountaboveLimit)/amountRecords)*100)
    print("invalid values in percentage", percentage)
    exit(0)