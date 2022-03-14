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
	something, _ := bufio.NewReader(c).ReadString('\n')
	return something
}

func HandleConnections(c net.Conn, request string) string {
	send_request(c)
	return rcv_response(c)
}

func main() {
}
