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
  m.update(message)
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
<h1>Error 400</h1>\
<p>Please fill in the name and message fields of the form.</p>\
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
    <div>\
  <iframe width=\"1280\" height=\"697\" src=\"https://www.youtube.com/embed/66BQ2ISoe_4\" title=\"YouTube video player\" frameborder=\"0\" allow=\"accelerometer; autoplay=1; mute=1; clipboard-write; encrypted-media; gyroscope; picture-in-picture\" allowfullscreen></iframe>\
  </div>\
  \
  </body>\
  </html> "
#eprint("HASH SCRIPT LAUNCHED")
form = cgi.FieldStorage()

if check_form_fields(form) < 0:
  print("HTTP/1.1 400 Bad Request")
  currentDate.printFormatedCurrentDate()
  print("Connection: Keep-Alive")
  print("Content-Type: text/html")
  print("Content-Length: " + str(len(error_body)))
  print("\n")
  print(error_body)

else:
  msg_hash = to_hash(str(form["user_message"]))
  ready_body = valid_body.replace("hash_placeholder", str(msg_hash))
  print("HTTP/1.1 200 OK")
  print("Content-Type: text/html")
  currentDate.printFormatedCurrentDate()
  print("Connection: Keep-Alive")
  print("Content-Length: " + str(len(ready_body)))
  print("\n")
  print(ready_body)