from __future__ import print_function
import cgi
import currentDate
import hashlib

import os, sys
#import cgitb;
#cgitb.enable()

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def to_hash(message):
  m = hashlib.sha256()
  m.update(message.encode('utf-8'))
  return m.hexdigest()

def check_form_fields(form):
  if "user_name" not in form or "user_message" not in form:
    return -1
  else:
    return 0

error_body = " <!DOCTYPE html>\
<html>\
<body>\
\
<h1>Error ERROR_PLACEHOLDER</h1>\
<p>MESSAGE_PLACEHOLDER</p>\
\
<button onclick=\"window.location.href='http://localhost:18000/hash.html';\">\
  Try Again\
</body>\
</html> "

valid_body = " <!DOCTYPE html>\
  <html>\
  <body>\
  \
  <h1>HASH</h1>\
  <p> Your message hash is: </p>\
  <p> hash_placeholder </p>\
  <button onclick=\"window.location.href='http://localhost:18000/hash.html';\">\
  AGAIN!\
  <button onclick=\"window.location.href='http://localhost:18000';\">\
  I WANNA GO HOME\
  </button>\
  \
  </body>\
  </html> "

def printErrorResponse(err, msg, error_body):
  error = err
  error_body = error_body.replace("ERROR_PLACEHOLDER", str(error))
  error_body = error_body.replace("MESSAGE_PLACEHOLDER", str(msg))
  print("HTTP/1.1 " + error)
  currentDate.printFormatedCurrentDate()
  print("Connection: Keep-Alive")
  print("Content-Type: text/html")
  print("Content-Length: " + str(len(error_body)))
  print("\n")
  print(error_body)

def printValidResponse(valid_body):
  print("HTTP/1.1 200 OK")
  currentDate.printFormatedCurrentDate()
  print("Content-Type: text/html")
  print("Connection: Keep-Alive")
  print("Content-Length: " + str(len(valid_body)))
  print("\n")
  print(valid_body)

#eprint("HASH SCRIPT LAUNCHED")
form = cgi.FieldStorage()

if check_form_fields(form) < 0:
  printErrorResponse("400 Bad Request", "Did you put a name and a message?", error_body)
else:
  try:
    msg_hash = to_hash(str(form["user_message"]))
    ready_body = valid_body.replace("hash_placeholder", str(msg_hash))
  except:
    printErrorResponse("502 Bad Gateway", "Did you put a name and a message?", error_body)
  else
    printValidResponse(valid_body)