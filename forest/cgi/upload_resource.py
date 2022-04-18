from __future__ import print_function

import currentDate
import cgi, os, sys
#import cgitb;
#cgitb.enable()

import sys

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)
# Response bodies
error_body = " <!DOCTYPE html>\
<html>\
  <head>\
    <h1>\
    Error 400 :(\
    </head>\
      </h1>\
<body>\
\
<p> Did you send a file ?</p>\
  <button onclick=\"window.location.href='http://localhost:18000/';\">\
  Home </button>\
  <button onclick=\"window.location.href='http://localhost:18000/upload_resource.html';\">\
  Send again</button>\
\
</body>\
</html> "

exception_body = " <!DOCTYPE html>\
<html>\
  <head>\
    <h1>\
    Error 500 :(\
    </head>\
      </h1>\
<body>\
\
<p> Something went wrong :(</p>\
  <button onclick=\"window.location.href='http://localhost:18000/';\">\
  Home </button>\
  <button onclick=\"window.location.href='http://localhost:18000/upload_resource.html';\">\
  Send again</button>\
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
<p> File uploaded successfully!</p>\
  <button onclick=\"window.location.href='http://localhost:18000/';\">\
  Home </button>\
  <button onclick=\"window.location.href='http://localhost:18000/upload_resource.html';\">\
  Send again</button>\
\
</body>\
</html> "

# Listing server files
root = "./forest/"
# Accessing form data
try:
  form = cgi.FieldStorage()
  # eprint("INFO: " + str(form))
  # Retrieving filename
  fileitem = form["filename"]
except:
  # if no file has been sent, error msg
  print("HTTP/1.1 500 Internal Server Error")
  currentDate.printFormatedCurrentDate()
  print("Connection: Keep-Alive")
  print("Content-Type: text/html")
  print("Content-Length: " + str(len(exception_body)))
  print("\n")
  print(exception_body)
else:
  if fileitem.filename:
    # Checking for leading path and striping it
    fn = os.path.basename(fileitem.filename.replace("\\", "/" ))
    # Copying uploaded files
    open("." + root + 'iAmUploadDir/' + fn, 'wb').write(fileitem.file.read())
    # Response Header and Body sent to stdout
    print("HTTP/1.1 200 OK")
    currentDate.printFormatedCurrentDate()
    print("Connection: Keep-Alive")
    print("Content-Type: text/html")
    print("Content-Length: " + str(len(valid_body)))
    print("\n")
    print(valid_body)
  else:
      # if no file has been sent, error msg
    print("HTTP/1.1 400 Bad Request")
    currentDate.printFormatedCurrentDate()
    print("Connection: Keep-Alive")
    print("Content-Type: text/html")
    print("Content-Length: " + str(len(error_body)))
    print("\n")
    print(error_body)