import datetime

# FORMAT PRINTED:
# "Date: Mon, 27 Jul 2009 12:28:53 GMT"
def formatDate(date):
  formated = "Date: " + date.strftime("%a") + ", "
  formated += date.strftime("%d") + " "
  formated += date.strftime("%b") + " "
  formated += date.strftime("%Y") + " "
  formated += date.strftime("%X") + " "
  formated += "GMT"
  return formated

def printFormatedCurrentTime():
  current_time = datetime.datetime.now()
  print(formatDate(current_time))

printFormatedCurrentTime()