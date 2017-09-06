package main

// ref: https://godoc.org/golang.org/x/net/ipv4#example-PacketConn--TracingIPPacketRoute

import (
	"fmt"
	"log"
	"net"
	"os"
	"runtime"
	"time"

	"golang.org/x/net/icmp"
	"golang.org/x/net/ipv4"
)

func inspectExecEnvironment() error {
	switch runtime.GOOS {
	case "darwin":
	case "linux":
		return nil
	default:
	}
	return fmt.Errorf("not supported on " + runtime.GOOS)
}

type path struct {
	SrcIP       string
	DstIP       string
	TTL         int
	TurnArround time.Duration
}

type pathSearcher struct {
	src    string
	lp     net.PacketConn
	p      *ipv4.PacketConn
	maxHop int
}

func (ps *pathSearcher) initialize() (err error) {
	ps.src = "0.0.0.0"
	ps.lp, err = net.ListenPacket("ip4:icmp", ps.src)
	if err != nil {
		return err
	}
	ps.p = ipv4.NewPacketConn(ps.lp)

	if err = ps.p.SetControlMessage(ipv4.FlagTTL|ipv4.FlagSrc|ipv4.FlagDst|ipv4.FlagInterface, true); err != nil {
		ps.lp.Close()
		return err
	}
	ps.maxHop = 32
	return nil
}

func (ps *pathSearcher) finalize() {
	ps.p.Close()
	ps.lp.Close()
}

func (ps *pathSearcher) validateICMPEchoReply(id int, ttl int, rm *icmp.Message) error {
	switch rm.Type {
	case ipv4.ICMPTypeEchoReply:
		if id != rm.Body.(*icmp.Echo).ID {
			return fmt.Errorf("expect: %d, got: %d", id, rm.Body.(*icmp.Echo).ID)
		}
		if ttl != rm.Body.(*icmp.Echo).Seq-1 {
			return fmt.Errorf("expect: %d, got: %d", ttl, rm.Body.(*icmp.Echo).Seq)
		}
	case ipv4.ICMPTypeTimeExceeded:
		return nil
	default:
		return fmt.Errorf("unknown ICMP message %+v", rm)
	}
	return nil
}

func (ps *pathSearcher) sendICMPEcho(dst string, ttl int) (*path, error) {
	rb := make([]byte, 1500)
	id := os.Getpid() & 0xffff
	wm := icmp.Message{
		Type: ipv4.ICMPTypeEcho,
		Code: 0,
		Body: &icmp.Echo{
			ID:   id,
			Seq:  ttl,
			Data: []byte("HELLO-R-U-THERE"),
		},
	}
	wb, err := wm.Marshal(nil)
	if err != nil {
		return nil, err
	}
	if err := ps.p.SetTTL(ttl); err != nil {
		return nil, err
	}

	begin := time.Now()
	if _, err := ps.p.WriteTo(wb, nil, &net.IPAddr{IP: net.ParseIP(dst)}); err != nil {
		return nil, err
	}
	if err := ps.p.SetReadDeadline(time.Now().Add(1 * time.Second)); err != nil {
		return nil, err
	}
	n, cm, peer, err := ps.p.ReadFrom(rb)
	if err != nil {
		if err, ok := err.(net.Error); ok && err.Timeout() {
			// TODO: do appropriate behavior
			log.Print("timeout!!!")
			return &path{"*", "*", ttl, 0}, nil
		}
		return nil, err
	}
	rtt := time.Since(begin)
	const icmpProtocolNumber = 1
	rm, err := icmp.ParseMessage(icmpProtocolNumber, rb[:n])
	if err != nil {
		return nil, err
	}
	if err := ps.validateICMPEchoReply(id, cm.TTL, rm); err != nil {
		return nil, err
	}
	return &path{ps.lp.LocalAddr().String(), peer.String(), cm.TTL, rtt}, nil
}

func (ps *pathSearcher) search(dst string) ([]*path, error) {
	paths := make([]*path, 0)
	for i := 1; i <= ps.maxHop; i++ {
		log.Print(i)
		path, err := ps.sendICMPEcho(dst, i)
		if err != nil {
			return nil, err
		}
		paths = append(paths, path)
		if path.DstIP == dst {
			break
		}
	}
	return paths, nil
}

func main() {
	if err := inspectExecEnvironment(); err != nil {
		log.Fatal(err)
	}

	var ps pathSearcher
	if err := ps.initialize(); err != nil {
		log.Fatal(err)
	}
	defer ps.finalize()

	var paths []*path
	var err error
	if paths, err = ps.search("182.22.31.124"); err != nil {
		log.Fatal(err)
	}
	for i := range paths {
		p := paths[i]
		log.Printf("Src: %s, Dst: %s, TTL: %d, Time[ms]: %d", p.SrcIP, p.DstIP, p.TTL, p.TurnArround.Nanoseconds()/1000/1000)
	}
}
