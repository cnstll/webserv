package main

import (
	"bufio"
	"bytes"
	"fmt"
	"io/ioutil"
	"net"
	"os"
	"os/exec"
	"strings"
	"sync"
)

type test_data struct {
	request  string
	response string
}

func GetCsvInfo(path string) []test_data {
	var data []test_data
	file, _ := os.Open(path)

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		var add = new(test_data)
		str := scanner.Text()
		parts := strings.Split(str, ",")
		add.request = parts[0]
		add.response = parts[1]
		data = append(data, *add)
	}
	return data
}

func send_request(c net.Conn) {

	text := `GET / HTTP/1.1
	Host: localhost:18000
	User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:98.0) Gecko/20100101 Firefox/98.0
	Accept-Language: en-US,en;q=0.5
	Accept-Encoding: gzip, deflate
	DNT: 1
	Connection: keep-alive
	Upgrade-Insecure-Requests: 1
	Sec-Fetch-Dest: document
	Sec-Fetch-Mode: navigate
	Sec-Fetch-Site: none
	Sec-Fetch-User: ?1
	Pragma: no-cache
	Cache-Control: no-cache`
	fmt.Fprintf(c, text+"\n")
}

func rcv_response(c net.Conn) string {
	var grosBiteArray [100000]byte
	rdr := bufio.NewReader(c)
	rdr.Read(grosBiteArray[:])
	//fmt.Printf("%s", grosBiteArray)
	return string(grosBiteArray[:])
}

func HandleConnections(c net.Conn, request string) string {
	send_request(c)

	return rcv_response(c)
}

var wg *sync.WaitGroup

func main() {
	nbOfConcurrentConnections := 20
	nbOfRequests := 20
	var outb bytes.Buffer
	server_root := os.Args[1]
	contentPath := server_root + "/index.html"
	content, _ := ioutil.ReadFile(contentPath)

	wg = new(sync.WaitGroup)

	for x := 0; x < nbOfConcurrentConnections; x++ {
		wg.Add(1)
		for x := 0; x < nbOfRequests; x++ {
			cmd := exec.Command("curl", "http://localhost:18000/")
			cmd.Stdout = &outb
			cmd.Run()
			if outb.String() != string(content) {
				fmt.Println(string(content))
				fmt.Print("\n\nVS\n\n")
				fmt.Println(string(outb.String()))
			}
			outb.Reset()
		}
	}
	for x := 0; x < nbOfConcurrentConnections; x++ {
		wg.Done()
	}
	wg.Wait()
	fmt.Println("If this message was not preceded by a deluge of scrambled html code, congrats! you")
}
