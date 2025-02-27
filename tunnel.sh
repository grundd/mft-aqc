# shell script must be first allowed: chmod +x tunnel.sh
#!/bin/bash
# to run it do:
# ./tunnel.sh

username="<your-username>"
sshuttle --dns -vr $username@lxtunnel.cern.ch 137.138.0.0/16 128.141.0.0/16 128.142.0.0/16 188.184.0.0/15 --python=python