from __future__ import print_function
import cgi
import currentDate

import os, sys
#import cgitb;
#cgitb.enable()

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def delete_resource(path):
  if not os.path.exists(path):
    return -1
  if os.path.isdir(path):
    os.rmdir(path)
  elif os.path.isfile(path):
    os.remove(path)
  else:
    return -1
  return 0

# Response bodies
error_body = " <!DOCTYPE html>\
<html>\
  <head>\
    <h1>\
    Error ERROR_PLACEHOLDER :(\
    </head>\
      </h1>\
<body>\
\
<p> MESSAGE_PLACEHOLDER </p>\
  <button onclick=\"window.location.href='http://localhost:18000/';\">\
  Home </button>\
  <button onclick=\"window.location.href='http://localhost:18000/delete_resource.html';\">\
  Try Again</button>\
\
</body>\
</html> "

valid_body = " <!DOCTYPE html>\
 <html>\
  <head>\
    <h1>\
    Success :)\
    </head>\
      </h1>\
<body>\
\
<p> File deleted!</p>\
  <button onclick=\"window.location.href='http://localhost:18000/';\">\
  Home </button>\
  <button onclick=\"window.location.href='http://localhost:18000/delete_resource.html';\">\
  Another One</button>\
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

#eprint("DELETE SCRIPT LAUNCHED")
try:
  env = os.environ
  qs = env.get('QUERY_STRING')
  if len(qs) == 0:
    printErrorResponse("400 Bad Request", "Illformed request :(", error_body)
  else:
    filename = qs.split("=")[1]
  root = env.get('PATH_INFO')
  path =  root + "/" + filename
  status = delete_resource(path)
  if (status < 0):
    printErrorResponse("404 Not Found", "File Not Found in working directory :(", error_body)
  else:
    printValidResponse(valid_body)
except:
  printErrorResponse("500 Internal Server Error", "<p> Oh woowww... something went slightly horribly wrong :S</p>", error_body)