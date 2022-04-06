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
<body>\
\
<h1>Error :( </h1>\
<p> Did you send a file ?</p>\
  <button onclick=\"window.location.href='http://localhost:18000/upload_files.html';\">\
  Send again\
\
</body>\
</html> "

valid_body = " <!DOCTYPE html>\
  <html>\
  <body>\
  \
  <h1>Success :)</h1>\
  <p> File successfully uploaded ! </p>\
  <button onclick=\"window.location.href='http://localhost:18000/upload_resource.html';\">\
  AGAIN!\
  <button onclick=\"window.location.href='http://localhost:18000';\">\
  I WANNA GO HOME\
  </button>\
  \
  </body>\
  </html> "

# Listing server files
root = "/core/server_root"
# Accessing form data
#eprint("ENV: " + str(os.environ))
form = cgi.FieldStorage()
# Retrieving filename
fileitem = form["filename"]
eprint("INFO: " + str(form))
if fileitem.filename:
  # Checking for leading path and striping it
  fn = os.path.basename(fileitem.filename.replace("\\", "/" ))
  # Copying uploaded files
  open("." + root + '/tmp/' + fn, 'wb').write(fileitem.file.read())
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
  print
  print(error_body)