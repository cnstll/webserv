from __future__ import print_function
import cgi
import currentDate

import os, sys
#import cgitb;
#cgitb.enable()

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def check_form_fields(form):
  if "file_to_delete" not in form:
    return -1
  else:
    return 0

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

#eprint("DELETE SCRIPT LAUNCHED")
try:
  form = cgi.FieldStorage()
except:
  error = "500 Internal Server Error"
  error_body = error_body.replace("ERROR_PLACEHOLDER", str(error))
  error_body = error_body.replace("MESSAGE_PLACEHOLDER", str("<p> Oh woowww... something went slightly horribly wrong :S</p>"))
  print("HTTP/1.1" + error)
  currentDate.printFormatedCurrentDate()
  print("Connection: Keep-Alive")
  print("Content-Type: text/html")
  print("Content-Length: " + str(len(error_body)))
  print("\n")
  print(error_body)
else:
  if check_form_fields(form) < 0:
    error = "400 Bad Request"
    error_body = error_body.replace("ERROR_PLACEHOLDER", str(error))
    error_body = error_body.replace("MESSAGE_PLACEHOLDER", str("<p></p>"))
    print("HTTP/1.1" + error)
    currentDate.printFormatedCurrentDate()
    print("Connection: Keep-Alive")
    print("Content-Type: text/html")
    print("Content-Length: " + str(len(error_body)))
    print("\n")
    print(error_body)

  else:
    tmp_folder = "/core/server_root/tmp"
    filename = form["file_to_delete"].value
    path =  "." + tmp_folder + "/" + str(filename)
    status = delete_resource(path)
    if (status < 0):
      error = "400 Bad Request"
      error_body = error_body.replace("ERROR_PLACEHOLDER", str(error))
      error_body = error_body.replace("MESSAGE_PLACEHOLDER", str("<p></p>"))
      print("HTTP/1.1" + error)
      currentDate.printFormatedCurrentDate()
      print("Connection: Keep-Alive")
      print("Content-Type: text/html")
      print("Content-Length: " + str(len(error_body)))
      print("\n")
      print(error_body)
    else:
      print("HTTP/1.1 200 OK")
      currentDate.printFormatedCurrentDate()
      print("Content-Type: text/html")
      print("Connection: Keep-Alive")
      print("Content-Length: " + str(len(valid_body)))
      print("\n")
      print(valid_body)