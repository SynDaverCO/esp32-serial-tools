/**
 * BOSSA.js is a JavaScript rewrite of BOSSAPY from Cura LE
 * BOSSAPY is python3 rewrite of https://github.com/shumatech/BOSSA
 *
 * BOSSA is a flash programming utility for Atmel's SAM family of flash-based
 * ARM microcontrollers. The motivation behind BOSSA is to create a simple,
 * easy-to-use, open source utility to replace Atmel's SAM-BA software. BOSSA
 * is an acronym for Basic Open Source SAM-BA Application to reflect that goal.
 *
 * @licstart
 *
 * Copyright (C) 2020, ShumaTech
 * Copyright (C) 2020 Ultimaker B.V.
 * Copyright (C) 2020 Aleph Objects, Inc.
 * Copyright (C) 2020 SynDaver Labs, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @licend
 */

#include <string>
#include <exception>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Samba.h"
#include "FlashFactory.h"
#include "Flasher.h"
#include "bossa.h"

void BOSSA::flash(File &firmware, SerialPort::Ptr port) {
    try {
        Samba samba;
        FlashFactory flashFactory;

        //samba.setDebug(true);
        if(samba.connect(port)) {
            fprintf(stderr, "Connected to SAM-BA\n");
            uint32_t chipId = samba.chipId();
            Flash::Ptr flash = flashFactory.create(samba, chipId);
            if (flash.get() == NULL) {
                fprintf(stderr, "Flash for chip ID %08x is not supported\n", chipId);
                return;
            } else {
                fprintf(stderr, "Found %s\n", flash->name().c_str());
            }
            std::string region;
            Flasher flasher(flash);
            flasher.lock(region,false);
            flasher.erase();
            flasher.lock(region,false);
            flasher.write(firmware);
            flasher.verify(firmware);
            flash->setBootFlash(true);
        } else {
            fprintf(stderr, "Failed to connect\n");
        }
    }
    catch (std::exception& e) {
        fprintf(stderr, "\n%s\n", e.what());
    }
    catch(...) {
        fprintf(stderr, "\nUnhandled exception\n");
    }
}
