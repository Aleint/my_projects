import time
import sqlite3

test_hash = input("Please insert the password hash you'd like to test: ")
dict_db = input("Filename containing the db file: ")
start_time = time.time()

# DB connection
con = sqlite3.connect(dict_db)
c = con.cursor()

# Hash lookup
c.execute('''
          SELECT pwd FROM dict_attack WHERE hash = ?
          ''', [test_hash])

# Retrieve the password if the dictionary contains the hash
result = c.fetchone()

if result != None:
    print("The password has been cracked --> "+result[0])
else:
    print("The attack failed!")

print("Time elapsed: %s s" % (time.time() - start_time))
