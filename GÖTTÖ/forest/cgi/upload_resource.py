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
    Error ERROR_PLACEHOLDER :(\
    </head>\
      </h1>\
<body>\
\
MESSAGE_PLACEHOLDER\
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

# Listing server files
upload_dir = os.environ.get("PATH_INFO")
# Accessing form data
try:
  form = cgi.FieldStorage()
  # eprint("INFO: " + str(env))
  # Retrieving filename
  fileitem = form["filename"]
except:
  # if no file has been sent, error msg
  printErrorResponse("500 Internal Server Error", "<p> Oh woowww... something went slightly horribly wrong :S</p>", error_body) 
else:
  if fileitem.filename:
    # Checking for leading path and striping it
    fn = os.path.basename(fileitem.filename.replace("\\", "/" ))
    # Copying uploaded files
    try:
      path = upload_dir + "/" + fn
      open(path, 'wb').write(fileitem.file.read())
    # Response Header and Body sent to stdout
    except:
      printErrorResponse("502 Bad Gateway", "<p></p>", error_body)
    else:
      printValidResponse(valid_body)
  else:
      # if no file has been sent, error msg
    printErrorResponse("400 Bad Request", "<p>Did you upload a file ?</p>", error_body)