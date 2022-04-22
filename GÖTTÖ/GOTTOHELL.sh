
echo "MAKE SURE YOUR SERVER IS RUNNING, AND RUNNING FAST"

go get github.com/charmbracelet/bubbletea
go get github.com/muesli/termenv
go get github.com/charmbracelet/bubbles/textinput

go build GOTTO.go
./GOTTO
