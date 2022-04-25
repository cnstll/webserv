# Webserv    

## Launch Webserv   
`make && ./webserv \[config_file_path\]`

## Testing from Firefox     
Go to http://localhost:18000 and clic on buttons.    
### Tested features:         
1/ Hash message      
2/ Upload       
3/ Delete   
4/ Redirects        
5/ Error testing     
### Suggested Config and root  
Conf: "/config/two_ports.config"     
rootDirOfServ: "./forest"   

## Testing with curl    
### Chunked uploads     
`curl -v -H "Transfer-Encoding: chunked" -F filename=@large_text_file.txt http://localhost:18000/upload_resource.py  `
`curl -v -H "Transfer-Encoding: chunked" -F filename=@large_video_file.mp4 http://localhost:18000/upload_resource.py ` 
### No Content Length    
`curl -v -H 'Content-Length:' -F filename=@small_text_file.txt http://localhost:18000/upload_resource.py     `

## Testing With Gotto   
Webserv can be tested with a Go tester named "GÖTTÖ".     
TUI done with [bubbleTea](https://github.com/charmbracelet/bubbletea).     
### Multiple tests can be performed in parallel:    
1/ Stress test  
2/ Static Pages integrity test  
3/ Upload files 
### Download GO dependencies
### Commands to build and Lauch the tester
``` Commands```
