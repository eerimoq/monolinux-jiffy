#!/usr/bin/env python3

import logging
import systest


LOGGER = logging.getLogger(__name__)


def main():
    sequencer = systest.setup("Monolinux Jiffy",
                              console_log_level=logging.DEBUG)

    sequencer.run()

    sequencer.report_and_exit()


main()
