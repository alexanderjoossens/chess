# run.py
"""
Program to be run from the command line to boot up the project. The argument -e is used to specify whether the receiver
(-e rx) or sender (-e tx) is launched.
"""
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-e", "--execute", help="selects boot mode", nargs=1, choices=["rx", "tx"])

args = parser.parse_args()

if args.execute:
    boot_mode = args.execute[0]
    if boot_mode == "rx":
        import boot.Receiver
        boot.Receiver.main()
    elif boot_mode == "tx":
        import boot.Sender
        boot.Sender.main()
