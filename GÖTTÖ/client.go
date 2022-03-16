package main

import (
	"bufio"
	"fmt"
	"net"
	"time"
)

func send_request(c net.Conn) {

	text := "hi server"
	fmt.Fprintf(c, text+"\n")
}

func rcv_response(c net.Conn) string {
	//func (b *Reader) Peek(n int) ([]byte, error)
	//func (b *Reader) ReadRune() (r rune, size int, err error)
	//func (b *Reader) Read(p []byte) (n int, err error)
	var grosBiteArray [100000]byte
	rdr := bufio.NewReader(c)
	r, _ := rdr.ReadFull(grosBiteArray[:])
	//c.ReadFull(rdr, grosBiteArray[:])
	fmt.Printf("%d - ", r)
	fmt.Printf(" %s\n", string(grosBiteArray[:]))
	//rdr.Reset(c)

	something := "dfghbjnkmn"
	return something
}

func HandleConnections(c net.Conn, request string) string {
	send_request(c)
	time.Sleep(1 * time.Second)
	return rcv_response(c)
}

func main() {
}
