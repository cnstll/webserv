package main

import (
	"bytes"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"os/exec"
	"strconv"
	"sync"
	"time"

	"github.com/charmbracelet/bubbles/stopwatch"
	tea "github.com/charmbracelet/bubbletea"
)

type model struct {
	stopwatch stopwatch.Model
	cursor    int
	choices   []string
	statuses  []string
	selected  map[int]struct{}
	testing   int
	randoshit int
}

type statusMsg int

type errMsg struct{ err error }

const url = "http://localhost:18000/"

func stressTestTest() tea.Msg {

	c := &http.Client{Timeout: 10 * time.Second}
	res, err := c.Get(url)
	if err != nil {
		return errMsg{err}
	}
	return statusMsg(res.StatusCode)
}

func (m model) Init() tea.Cmd {
	return stressTestTest
}

func uploadTest() int {

	//! the files here are prety small for ease of testing this tester out, they should be replaced with larger files
	var testFiles = [5]string{"small_text_file.txt", "large_text_file.txt", "img.jpeg", "img.jpeg", "img.jpeg"}
	OgContentRootPath := "/mnt/nfs/homes/jescully/Documents/webserv/GÖTTÖ/upload_files_originals/"
	uploadContentPath := "/mnt/nfs/homes/jescully/Documents/webserv/core/server_root/tmp/"

	for _, file := range testFiles {
		cmd := exec.Command("curl", "-F", "filename=@/mnt/nfs/homes/jescully/Documents/webserv/GÖTTÖ/upload_files_originals/"+file, "http://localhost:18000/upload_resource.py")
		cmd.Run()
		OgContent, _ := ioutil.ReadFile(OgContentRootPath + file)
		UploadedContent, _ := ioutil.ReadFile(uploadContentPath + file)
		if string(UploadedContent) != string(OgContent) {
			return -1
			// fmt.Print("\n\nUPLOAD TEST FAIL\n\n")
			// fmt.Print("ON FILE: ")
			// fmt.Print(OgContentRootPath + file + "\n")
		}
	}
	return 0
}

var wg *sync.WaitGroup

func stressTest(m model) int {
	nbOfConcurrentConnections := 10
	nbOfRequests := 1
	m.statuses[0] = "TESTED"

	wg = new(sync.WaitGroup)

	for x := 0; x < nbOfConcurrentConnections; x++ {
		wg.Add(1)
		for x := 0; x < nbOfRequests; x++ {
			cmd := exec.Command("curl", "http://localhost:18000/")
			cmd.Run()
		}
	}
	for x := 0; x < nbOfConcurrentConnections; x++ {
		wg.Done()
	}
	wg.Wait()
	return 0
}

func staticPagesTest(m model) int {
	nbOfConcurrentConnections := 10
	nbOfRequests := 100
	var outb bytes.Buffer
	server_root := "../core/server_root"
	contentPath := server_root + "/index.html"
	content, _ := ioutil.ReadFile(contentPath)
	fail := 0

	wg = new(sync.WaitGroup)

	for x := 0; x < nbOfConcurrentConnections; x++ {
		wg.Add(1)
		for x := 0; x < nbOfRequests; x++ {
			cmd := exec.Command("curl", "http://localhost:18000/")
			cmd.Stdout = &outb
			cmd.Run()
			if outb.String() != string(content) {
				fail = 1
				break
			}
			outb.Reset()
		}
	}
	for x := 0; x < nbOfConcurrentConnections; x++ {
		wg.Done()
	}
	wg.Wait()

	if fail == 1 {
		return -1
	}
	return 0
}

func (m model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	if m.testing == 1 {
		for i, _ := range m.choices {
			if _, ok := m.selected[i]; ok {
				if i == 0 {
					stat := stressTest(m)
					if stat == 0 {
						m.statuses[0] = "PASSED"
					} else {
						m.statuses[0] = "FAILED"
					}
				} else if i == 2 {
					stat := uploadTest()
					if stat == 0 {
						m.statuses[2] = "PASSED"
					} else {
						m.statuses[2] = "FAILED"
					}
				} else if i == 1 {
					stat := staticPagesTest(m)
					if stat == 0 {
						m.statuses[1] = "PASSED"
					} else {
						m.statuses[1] = "FAILED"
					}
				}
			}
		}
		m.testing = 0
		return m, nil
	}
	switch msg := msg.(type) {
	case statusMsg:
		status := int(msg)
		m.statuses[0] = strconv.Itoa(status)
	case tea.KeyMsg:
		switch msg.String() {
		case "ctrl+c", "q":
			return m, tea.Quit
		case "up", "k":
			if m.cursor > 0 {
				m.cursor--
			}
		case "down", "j":
			if m.cursor < len(m.choices)-1 {
				m.cursor++
			}
		case " ", "x":
			_, ok := m.selected[m.cursor]
			if ok {
				delete(m.selected, m.cursor)
			} else {
				m.selected[m.cursor] = struct{}{}
			}
		case "enter":
			for i := range m.choices {
				if _, ok := m.selected[i]; ok {
					m.statuses[i] = "testing ..."
					stressTestTest()
				}
			}
			m.testing = 1
		}
	}
	return m, stressTestTest
}

func (m model) selectView() string {
	s := "What should we test today?\n\n"

	for i, choice := range m.choices {
		cursor := " "
		if m.cursor == i {
			cursor = ">"
		}

		checked := " "
		if _, ok := m.selected[i]; ok {
			checked = "x"
		}
		s += fmt.Sprintf("%s [%s] %s %s\n", cursor, checked, choice, m.statuses[i])
	}

	s += "\nPress x to select\n"
	s += "\nPress enter to start\n"
	s += "\nPress q to quit.\n"

	return s
}

func (m model) View() string {
	s := ""
	s = m.selectView()
	return s
}

func main() {
	m := model{
		choices:  []string{"Stress test (Don't crash!)", "Static pages check", "Upload files", "delete files"},
		statuses: []string{"", "", "", ""},

		// A map which indicates which choices are selected. We're using
		// the  map like a mathematical set. The keys refer to the indexes
		// of the `choices` slice, above.
		selected: make(map[int]struct{}),
	}
	p := tea.NewProgram(m, tea.WithAltScreen())
	err := p.Start()
	if err != nil {
		fmt.Printf("Alas, there's been an error: %v", err)
		os.Exit(1)
	}
}
