tar --exclude-backups --exclude-vcs  --transform 's/^\./dcmtk-3.6.4/' -zcvf ~/rpmbuild/SOURCES/dcmtk-3.6.4.tar.gz .
rpmbuild -ba dcmtk.spec
