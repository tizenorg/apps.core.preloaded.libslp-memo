#sbs-git:slp/apps/l/libslp-memo libslp-memo 0.1.4 e4fc8040f9e5243551bdaa7748006516d5c94e90
Name:       libslp-memo
Summary:    Memo DB interface library
Version: 0.1.4
Release:    21
Group:      TO_BE/FILLED_IN
License:    TO BE FILLED IN
Source0:    libslp-memo-%{version}.tar.gz
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

%build
export LDFLAGS+=" -Wl,--rpath=/usr/lib -Wl,--as-needed -Wl,--hash-style=both"
cmake . -DCMAKE_INSTALL_PREFIX=/usr

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%post
mkdir -p /opt/dbspace
sqlite3 /opt/dbspace/.memo.db 'PRAGMA journal_mode = PERSIST;
CREATE TABLE if not exists memo ( id INTEGER PRIMARY KEY autoincrement, content TEXT, written_time TEXT, create_time INTEGER, modi_time INTEGER, delete_time INTEGER, doodle INTEGER, color INTEGER, comment TEXT, favorite INTEGER,font_respect INTEGER, font_size INTEGER, font_color INTEGER, doodle_path TEXT );
                              '

# Change file owner
chown :db_memo /opt/dbspace/.memo.db
chown :db_memo /opt/dbspace/.memo.db-journal

# Change file permissions
chmod 660  /opt/dbspace/.memo.db
chmod 660  /opt/dbspace/.memo.db-journal

%files
%defattr(-,root,root,-)
%{_libdir}/*.so.*
%attr(660,-,db_memo) /opt/data/libslp-memo/.LIBSLP_MEMO_DB_CHANGED

%files devel
%defattr(-,root,root,-)
%{_libdir}/*.so
%{_libdir}/pkgconfig/memo.pc
%{_includedir}/*.h

