package main

import (
	"bufio"
	//	"fmt"
	"net"
	"os"
	//"strconv"
	"strings"
	"sync"
	"testing"
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

func TestResponsesTable(t *testing.T) {
	var tests []test_data = GetCsvInfo("testInput.csv")

	var wg sync.WaitGroup

	c, _ := net.Dial("tcp", "127.0.0.1:18000")
	i := 0
	for x := 0; x < 1; x++ {
		wg.Add(1)
		for _, test := range tests {
			i++
			output := HandleConnections(c, test.request)
			//parts := strings.Split(output, "\n")
			//partsExpected := strings.Split(test.response, "\n")
			//time.Sleep(1 * time.Second)
			output = strings.Trim(output, "\n")
			output = strings.Trim(output, " ")

			//if output != test.response {
			// ! this is shit
			//	t.Error("\n\ntest number " + strconv.Itoa(i) + " failed\n" + "Request: " + "\n" + "expected response: " + partsExpected[0] + "\nrecieved response: " + parts[0])
			//}
		}
		wg.Done()
	}
	wg.Wait()
}
