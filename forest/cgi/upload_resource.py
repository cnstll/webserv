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
    Error PLACEHOLDER :(\
    </head>\
      </h1>\
<body>\
\
MESSAGE\
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
upload_dir = os.environ.get("PATH_INFO")
# Accessing form data
try:
  form = cgi.FieldStorage()
  # eprint("INFO: " + str(env))
  # Retrieving filename
  fileitem = form["filename"]
except:
  # if no file has been sent, error msg
  error = "500 Internal Server Error"
  error_body = error_body.replace("PLACEHOLDER", str(error))
  error_body = error_body.replace(
      "MESSAGE", str("<p></p>"))
  print("HTTP/1.1" + error)
  currentDate.printFormatedCurrentDate()
  print("Connection: Keep-Alive")
  print("Content-Type: text/html")
  print("Content-Length: " + str(len(error_body)))
  print("\n")
  print(error_body)
else:
  if fileitem.filename:
    # Checking for leading path and striping it
    fn = os.path.basename(fileitem.filename.replace("\\", "/" ))
    # Copying uploaded files
    try:
      open(upload_dir + "/" + fn, 'wb').write(fileitem.file.read())
    # Response Header and Body sent to stdout
    except:
      error = "502 Bad Gateway"
      error_body = error_body.replace("PLACEHOLDER", str(error))
      error_body = error_body.replace("MESSAGE", str("<p></p>"))
      print("HTTP/1.1 " + error)
      currentDate.printFormatedCurrentDate()
      print("Connection: Keep-Alive")
      print("Content-Type: text/html")
      print("Content-Length: " + str(len(error_body)))
      print("\n")
      print(error_body)
    else:
      print("HTTP/1.1 200 OK")
      currentDate.printFormatedCurrentDate()
      print("Connection: Keep-Alive")
      print("Content-Type: text/html")
      print("Content-Length: " + str(len(valid_body)))
      print("\n")
      print(valid_body)

  else:
      # if no file has been sent, error msg
    error = "400 Bad Request"
    error_body = error_body.replace("PLACEHOLDER", str(error))
    error_body = error_body.replace("MESSAGE", str("<p>Did you upload a file ?</p>"))
    print("HTTP/1.1" + error)
    currentDate.printFormatedCurrentDate()
    print("Connection: Keep-Alive")
    print("Content-Type: text/html")
    print("Content-Length: " + str(len(error_body)))
    print("\n")
    print(error_body)