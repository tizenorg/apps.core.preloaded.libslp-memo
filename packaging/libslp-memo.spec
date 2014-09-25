Name:       libslp-memo
Summary:    Memo DB interface library
Version:    0.1.5
Release:    0
Group:      Application Framework/Libraries
License:    Flora
Source0:    libslp-memo-%{version}.tar.gz
Source1001: libslp-memo.manifest
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(heynoti)
BuildRequires:  pkgconfig(db-util)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(sqlite3)
BuildRequires:  pkgconfig(libtzplatform-config)
BuildRequires:  cmake

%description
library for memo(development package)

%package devel
Summary:    Development for %{name}
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
%__make %{?_smp_mflags}

%install
%make_install

%post
mkdir -p %{TZ_SYS_DB}
sqlite3 %{TZ_SYS_DB}/.memo.db 'PRAGMA journal_mode = PERSIST;
CREATE TABLE if not exists memo ( id INTEGER PRIMARY KEY autoincrement, content TEXT, written_time TEXT, create_time INTEGER, modi_time INTEGER, delete_time INTEGER, doodle INTEGER, color INTEGER, comment TEXT, favorite INTEGER,font_respect INTEGER, font_size INTEGER, font_color INTEGER, doodle_path TEXT );
                              '
/sbin/ldconfig

# Change file owner
chown :%{TZ_SYS_USER_GROUP} %{TZ_SYS_DB}/.memo.db
chown :%{TZ_SYS_USER_GROUP} %{TZ_SYS_DB}/.memo.db-journal

# Change file permissions
chmod 660  %{TZ_SYS_DB}/.memo.db
chmod 660  %{TZ_SYS_DB}/.memo.db-journal

chsmack -a 'User' %{TZ_SYS_DB}/.memo.db*

%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/*.so.*

%files devel
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/*.so
%{_libdir}/pkgconfig/memo.pc
%{_includedir}/*.h
