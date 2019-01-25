#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25
# @version $Id$

# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# go to additional mode
netedit.additionalMode()

# select E1
netedit.changeAdditional("e1Detector")

# set a invalid  frequency (dummy)
netedit.modifyAdditionalDefaultValue(2, "dummyFrequency")

# try to create E1 with invalid frequency
netedit.leftClick(referencePosition, 250, 250)

# set a invalid  frequency
netedit.modifyAdditionalDefaultValue(2, "-30")

# try to create E1 with invalid frequency
netedit.leftClick(referencePosition, 250, 250)

# set a valid frequency
netedit.modifyAdditionalDefaultValue(2, "150")

# create E1 with valid frequency
netedit.leftClick(referencePosition, 250, 250)

# Check undo redo
netedit.undo(referencePosition, 1)
netedit.redo(referencePosition, 1)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
