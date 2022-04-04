line = " HELLO WORLD"
numOfLine = 10000
with open('my_file.txt','w') as out:
  for i in range(numOfLine):
    l = str(i) + line
    out.write('{}\n'.format(l))
