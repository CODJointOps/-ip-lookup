#!/bin/bash

rpmbuild -bb ip-lookup.spec --define "_sourcedir $(pwd)" --define "_specdir $(pwd)" --define "_builddir $(pwd)" --define "_srcrpmdir $(pwd)" --define "_rpmdir $(pwd)"

RPM_FILE=$(find $(pwd)/x86_64 -type f -name '*.rpm' ! -name '*.src.rpm')

if [ -n "$RPM_FILE" ]; then
    echo "Installing $RPM_FILE..."
    sudo dnf install "$RPM_FILE"
else
    echo "No RPM package found to install."
fi
