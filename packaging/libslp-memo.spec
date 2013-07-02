Name:       libslp-memo
Summary:    Memo DB interface library
Version:    0.1.5
Release:    1
Group:      Applications/Libraries
License:    Flora
Source0:    libslp-memo-%{version}.tar.gz
Source1001: 	libslp-memo.manifest
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(heynoti)
BuildRequires:  pkgconfig(db-util)
BuildRequires:  pkgconfig(vconf)

BuildRequires:  cmake
Requires(post): /usr/bin/sqlite3

%description
library for memo(development package)

%package devel
Summary:    Development files for %{name}
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
Development files for %{name}

%prep
%setup -q
cp %{SOURCE1001} .

%build
export LDFLAGS+=" -Wl,--rpath=%{_libdir} -Wl,--as-needed -Wl,--hash-style=both"
%cmake .

make %{?jobs:-j%jobs}

%install
%make_install

%post
mkdir -p /opt/dbspace
sqlite3 /opt/dbspace/.memo.db 'PRAGMA journal_mode = PERSIST;
CREATE TABLE if not exists memo ( id INTEGER PRIMARY KEY autoincrement, content TEXT, written_time TEXT, create_time INTEGER, modi_time INTEGER, delete_time INTEGER, doodle INTEGER, color INTEGER, comment TEXT, favorite INTEGER,font_respect INTEGER, font_size INTEGER, font_color INTEGER, doodle_path TEXT );
                              '

if [ -f /usr/lib/rpm-plugins/msm.so ]
then
#chsmack -a 'memo::db' /opt/usr/dbspace/.memo.db*
fi



# Change file owner
chown :5000 /opt/dbspace/.memo.db
chown :5000 /opt/dbspace/.memo.db-journal

# Change file permissions
chmod 660  /opt/dbspace/.memo.db
chmod 660  /opt/dbspace/.memo.db-journal

%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%manifest libslp-memo.manifest
%{_libdir}/*.so.*

%files devel
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/*.so
%{_libdir}/pkgconfig/memo.pc
%{_includedir}/*.h
