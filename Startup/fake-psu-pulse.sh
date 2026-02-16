#!/bin/bash
echo "GOTOSTATE1"
echo -e "Destination=StateMachine\nFunction=GOTOSTATE1" | nc 127.0.0.1 24680
sleep 1
echo "GOTOSTATE2"
echo -e "Destination=StateMachine\nFunction=GOTOSTATE2" | nc 127.0.0.1 24680
sleep 1
echo "GOTOSTATE3"
echo -e "Destination=StateMachine\nFunction=GOTOSTATE3" | nc 127.0.0.1 24680
sleep 5
echo "GOTOSTATE1"
echo -e "Destination=StateMachine\nFunction=GOTOSTATE1" | nc 127.0.0.1 24680
