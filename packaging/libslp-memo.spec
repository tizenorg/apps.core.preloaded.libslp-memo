Name:       libslp-memo
Summary:    Memo DB interface library
Version:	0.1.4
Release:    1
Group:      Libraries
License:    Flora Software License
Source0:    libslp-memo-%{version}.tar.gz
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(db-util)
BuildRequires:  cmake
Requires(post): /usr/bin/sqlite3, /bin/mkdir, /usr/bin/vconftool

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
%make_install

%post
#add db
vconftool set -t int db/memo/data-change 0 -g 5000

test -f /opt/dbspace/.memo.db || sqlite3 /opt/dbspace/.memo.db 'PRAGMA journal_mode = PERSIST;
CREATE TABLE if not exists memo ( id INTEGER PRIMARY KEY autoincrement, content TEXT, written_time TEXT, create_time INTEGER, modi_time INTEGER, delete_time INTEGER, doodle INTEGER, color INTEGER, comment TEXT, favorite INTEGER, font_respect INTEGER, font_size INTEGER, font_color INTEGER, doodle_path TEXT );
'
# Change file owner
chown :6009 /opt/dbspace/.memo.db
chown :6009 /opt/dbspace/.memo.db-journal

# Change file permissions
chmod 660  /opt/dbspace/.memo.db
chmod 660  /opt/dbspace/.memo.db-journal

%files
%{_libdir}/*.so.*
%attr(660,root,db_memo) /opt/data/libslp-memo/.LIBSLP_MEMO_DB_CHANGED

%files devel
%{_libdir}/*.so
%{_libdir}/pkgconfig/memo.pc
%{_includedir}/*.h


