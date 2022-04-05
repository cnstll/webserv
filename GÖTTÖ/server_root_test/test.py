
string = " <!DOCTYPE html>\
<html>\
<body>\
\
<h1>My First Heading</h1>\
<p>My first paragraph.</p>\
\
</body>\
</html> "

print "HTTP/1.1 200 OK"
print "Date: Mon, 27 Jul 2009 12:28:53 GMT"
print "Connection: Keep-Alive"
print "Content-Length: " + str(len(string))
print "\n"
print string