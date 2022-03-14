package main

import (
	"bufio"
	//	"fmt"
	"net"
	"os"
	"strconv"
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
	var tests []test_data = GetCsvInfo("expected_vs_output.csv")

	var wg sync.WaitGroup

	c, _ := net.Dial("tcp", "127.0.0.1:18001")
	i := 0
	for x := 0; x < 90; x++ {
		wg.Add(1)
		for _, test := range tests {
			i++
			output := HandleConnections(c, test.request)
			output = strings.Trim(output, "\n")
			output = strings.Trim(output, " ")
			if output != test.response {
				t.Error("\n\ntest number " + strconv.Itoa(i) + " failed\n" + "Request: " + test.request + "\n" + "expected response: " + test.response + "\nrecieved response: " + output)
			}
		}
		wg.Done()
	}
	wg.Wait()
}
