package main

// ref: https://stackoverflow.com/questions/2937123/implementing-icmp-ping-in-go

import (
	"log"
	"net"
	"os"
	"runtime"

	"golang.org/x/net/icmp"
	"golang.org/x/net/ipv4"
)

const hostIP = "0.0.0.0"
const targetIP = "192.168.100.254"
const icmpProtocolNumber = 1

func main() {
	switch runtime.GOOS {
	case "darwin":
	case "linux":
		log.Println("you may need to adjust the net.ipv4 ping_group_range kernel state")
	default:
		log.Println("not supported on", runtime.GOOS)
		return
	}

	c, err := icmp.ListenPacket("ip4:icmp", hostIP)
	if err != nil {
		log.Fatal(err)
	}
	defer c.Close()

	wm := icmp.Message{
		Type: ipv4.ICMPTypeEcho,
		Code: 0,
		Body: &icmp.Echo{
			ID:   os.Getpid() & 0xffff,
			Seq:  1,
			Data: []byte("HELLO-R-U-THERE"),
		},
	}
	wb, err := wm.Marshal(nil)
	if err != nil {
		log.Fatal(err)
	}
	_, err = c.WriteTo(wb, &net.IPAddr{IP: net.ParseIP(targetIP)})
	if err != nil {
		log.Fatal(err)
	}

	rb := make([]byte, 1500)
	n, peer, err := c.ReadFrom(rb)
	if err != nil {
		log.Fatal(err)
	}
	rm, err := icmp.ParseMessage(icmpProtocolNumber, rb[:n])
	if err != nil {
		log.Fatal(err)
	}
	switch rm.Type {
	case ipv4.ICMPTypeEchoReply:
		log.Printf("got reflection from %v", peer)
	default:
		log.Printf("got %+v; want echo reply", rm)
	}
}
