package main

import (
	"bufio"
	"bytes"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"os/exec"
	"strings"
	"sync"
	"time"

	"github.com/charmbracelet/bubbles/textinput"
	tea "github.com/charmbracelet/bubbletea"
	"github.com/muesli/termenv"
)

var (
	term    = termenv.EnvColorProfile()
	keyword = makeFgStyle("211")
	subtle  = makeFgStyle("241")
	dot     = colorFg(" • ", "236")

	// Gradient colors we'll use for the progress bar

)

// Color a string's foreground with the given value.
func colorFg(val, color string) string {
	return termenv.String(val).Foreground(term.Color(color)).String()
}

// Return a function that will colorize the foreground of a given string.
func makeFgStyle(color string) func(string) string {
	return termenv.Style{}.Foreground(term.Color(color)).Styled
}

// Color a string's foreground and background with the given value.
func makeFgBgStyle(fg, bg string) func(string) string {
	return termenv.Style{}.
		Foreground(term.Color(fg)).
		Background(term.Color(bg)).
		Styled
}

type model struct {
	textInput  textinput.Model
	cursor     int
	choices    []string
	statuses   []string
	selected   map[int]struct{}
	testing    int
	url        string
	funkTable  []func(m model)
	typing     bool
	toBeTested [4]bool
}

type statusMsg struct {
	testType int
	status   string
}

type errMsg struct{ err error }

func picker(m model) tea.Cmd {
	if m.toBeTested[0] {
		return checkSomeUrl(m)
	}
	if m.toBeTested[1] {
		return checkSomeStatic(m)
	}
	if m.toBeTested[2] {
		return uploadFiles(m)
	}
	//return checkSomeStatic(m)
	return func() tea.Msg {
		return statusMsg{5, ""}
	}
}

var wg *sync.WaitGroup

func checkSomeUrl(m model) tea.Cmd {
	return func() tea.Msg {
		if m.toBeTested[0] {

			if m.url != "" {
				nbOfConnections := 10000
				wg = new(sync.WaitGroup)
				for i := 0; i < nbOfConnections; i++ {
					wg.Add(1)
					go func() {
						defer wg.Done()
						c := &http.Client{Timeout: 10 * time.Second}
						c.Get(m.url)
					}()
				}
				wg.Wait()
			}
			return statusMsg{0, "PASS"}
		}
		return statusMsg{0, m.statuses[0]}
	}
}

func checkSomeStatic(m model) tea.Cmd {

	contentPath := "./server_root_test/index.html"
	content, _ := ioutil.ReadFile(contentPath)
	fileRet, _ := os.Create("./log/ServerResponse.txt")
	file, _ := os.Create("./log/OriginalFile.txt")
	writer := bufio.NewWriter(file)
	writerRet := bufio.NewWriter(fileRet)

	return func() tea.Msg {
		if m.toBeTested[1] {
			if m.url != "" {
				c := &http.Client{Timeout: 5 * time.Second}
				ret, err := c.Get(m.url)

				if err != nil {
					return statusMsg{1, "OOPS something went wrong #500 internat server tester error"}
				}
				defer ret.Body.Close()
				buf := new(bytes.Buffer)
				buf.ReadFrom(ret.Body)
				newStr := buf.String()
				if newStr == string(content) {
					return statusMsg{1, "PASS"}
				} else {
					writer.WriteString(string(content))
					writerRet.WriteString(newStr)
					writer.Flush()
					writerRet.Flush()
					return statusMsg{1, "FAIL"}
				}
			} else {
				return statusMsg{1, "FAIL"}
			}
		}
		return statusMsg{1, m.statuses[1]}
	}
}

// func checkSomeStatic(m model) tea.Cmd {
// 	contentPath := "./server_root_test/index.html"
// 	content, _ := ioutil.ReadFile(contentPath)
// 	fileRet, _ := os.Create("./log/ServerResponse.txt")
// 	file, _ := os.Create("./log/OriginalFile.txt")
// 	writer := bufio.NewWriter(file)
// 	writerRet := bufio.NewWriter(fileRet)
// 	return func() tea.Msg {
// 		if m.toBeTested[1] {
// 			if m.url != "" {
// 				// c := &http.Client{Timeout: 5 * time.Second}
// 				resp, err := http.PostForm(m.url+"/hash.html", url.Values{"key": {"Value"}, "id": {"123"}})
// 				if err != nil {
// 					return statusMsg{1, "OOPS something went wrong #500 internat server tester error"}
// 				}
// 				defer ret.Body.Close()
// 				buf := new(bytes.Buffer)
// 				buf.ReadFrom(ret.Body)
// 				newStr := buf.String()
// 				if newStr == string(content) {
// 					return statusMsg{1, "PASS"}
// 				} else {
// 					writer.WriteString(string(content))
// 					writerRet.WriteString(newStr)
// 					writer.Flush()
// 					writerRet.Flush()
// 					return statusMsg{1, "FAIL"}
// 				}
// 			} else {
// 				return statusMsg{1, "FAIL"}
// 			}
// 		}
// 		return statusMsg{1, m.statuses[1]}
// 	}
// }

func uploadFiles(m model) tea.Cmd {

	//! the files here are prety small for ease of testing this tester out, they should be replaced with larger files
	var testFiles = [5]string{"small_text_file.txt", "large_text_file.txt", "large_video_file.mp4", "img.jpeg", "img.jpeg"}
	OgContentRootPath := "/mnt/nfs/homes/jescully/Documents/webserv/GÖTTÖ/upload_files_originals/"
	uploadContentPath := "/mnt/nfs/homes/jescully/Documents/webserv/core/server_root/tmp/"

	return func() tea.Msg {
		if m.toBeTested[2] {

			for _, file := range testFiles {
				cmd := exec.Command("curl", "-F", "filename=@/mnt/nfs/homes/jescully/Documents/webserv/GÖTTÖ/upload_files_originals/"+file, "http://localhost:18000/upload_resource.py")
				cmd.Run()
				OgContent, _ := ioutil.ReadFile(OgContentRootPath + file)
				UploadedContent, _ := ioutil.ReadFile(uploadContentPath + file)
				if string(UploadedContent) != string(OgContent) {
					return statusMsg{2, "FAIL"}
				}
			}
			return statusMsg{2, "PASS"}
		}
		return statusMsg{2, m.statuses[2]}
	}
}

func (m model) Init() tea.Cmd {
	return textinput.Blink
}

func (m model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	switch msg := msg.(type) {
	case statusMsg:
		x := msg
		m.statuses[x.testType] = x.status
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
		case "esc":
			m.typing = true
		case "enter":
			if m.typing {
				m.url = strings.TrimSpace(m.textInput.Value())
				if m.url != "" {
					m.typing = false
				}
			} else {
				for i := range m.choices {
					if _, ok := m.selected[i]; ok {
						m.statuses[i] = "testing ..."
						m.toBeTested[i] = true
					} else {
						m.toBeTested[i] = false
					}
				}
				return m, tea.Batch(
					checkSomeStatic(m),
					checkSomeUrl(m),
					uploadFiles(m),
				)
			}
		}
	}
	if m.typing {
		var cmd tea.Cmd
		m.textInput, cmd = m.textInput.Update(msg)
		return m, cmd
	}
	return m, nil
}

func (m model) selectView() string {
	s := ""
	// prompt := "WHat Should we test?"
	s += fmt.Sprintf("What should we test today?\n\n")

	for i, choice := range m.choices {
		cursor := " "
		if m.cursor == i {
			cursor = keyword(">")
		}
		checked := " "
		if _, ok := m.selected[i]; ok {
			checked = keyword("x")
			choice = keyword(m.choices[i])
		}
		s += fmt.Sprintf("%s [%s] %s %s\n", cursor, checked, choice, m.statuses[i])
	}

	s += "\nTesting on URL: " + keyword(m.url) + "\n"
	s += subtle("\nPress x to select\n")
	s += subtle("\nPress enter to start\n")
	s += subtle("\nPress esc to modify url\n")
	s += subtle("\nPress q to quit.\n")

	return s
}

func (m model) View() string {
	// s := ""
	if m.typing {
		return fmt.Sprintf("Enter Server URL:\n%s", m.textInput.View())
	}
	return fmt.Sprintf(m.selectView())
}

func main() {
	t := textinput.NewModel()
	t.Placeholder = "http://localhost:18000"
	t.Focus()

	m := model{
		textInput:  t,
		choices:    []string{"Stress test (Don't crash!)", "Static pages check", "Upload files", "delete files"},
		statuses:   []string{"", "", "", ""},
		selected:   make(map[int]struct{}),
		typing:     true,
		toBeTested: [4]bool{false, false, false, false},
	}
	p := tea.NewProgram(m, tea.WithAltScreen())
	err := p.Start()
	if err != nil {
		fmt.Printf("Alas, there's been an error: %v", err)
		os.Exit(1)
	}
}
