#!/usr/bin/python3
import sys
from typing import Iterable
sys.path.append('/usr/local/lib/python3.8/dist-packages/')
import mysql.connector
from mysql.connector import errorcode

def MysqlConnect(username, pwd,hostName, databaseName):
    try:
        connection = mysql.connector.connect(user=username, password=pwd, host=hostName, database=databaseName, auth_plugin='mysql_native_password')
        
    except mysql.connector.connect.Error as err:
        if err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
            print("user / pass\n")
            exit(1)
        if err.errno == errorcode.ER_BAD_DB_ERROR:
            print("database\n")
            exit(1)
        else:
            print(err)
            exit(1)
    return connection
   
def MysqlQueryReturn(query, connection):
    
    
    cursor = connection.cursor()
    cursor.execute(query)
    result = cursor.fetchall()
    data = [row[0] for row in result]
    return data
    
def MysqlQueryInsert(query, connection):
    cursor = connection.cursor()
    cursor.execute(query)
    connection.commit()
    cursor.close()






  