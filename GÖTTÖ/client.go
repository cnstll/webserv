package main

import (
	"bufio"
	"fmt"
	"net"
)

func send_request(c net.Conn) {

	text := "hi server"
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
	//time.Sleep(1 * time.Second)
	return rcv_response(c)
}

func main() {
}
