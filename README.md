# 🌿 Leave Request Management System (LRMS)

A robust and user-friendly terminal-based leave management system built in C, designed to streamline employee leave requests and manager approvals with secure authentication and data persistence.


---

## ✨ Features

### 👤 **For Employees**
- **Secure Authentication** – Sign up and log in with password masking
- **Leave Requests** – Submit Annual or Urgent leave requests
- **Leave History** – View all past requests with approval status
- **Profile Management** – Update personal information
- **Request Management** – Delete pending leave requests before approval

### 👔 **For Managers**
- **Leave Approvals** – Approve or reject employee leave requests
- **User Management** – Review and approve new employee sign-ups
- **Records Access** – View comprehensive employee leave histories
- **Statistics** – Generate visual leave statistics and analytics
- **HR Controls** – Delete user accounts (HR Manager exclusive)

### 🔧 **System Features**
- **Secure Password Handling** – Protected input with recovery via security questions
- **Data Persistence** – Binary file storage for all data
- **Cross-Platform** – Compatible with Windows and Unix-based systems
- **Enhanced UI** – Color-coded terminal interface using ANSI codes

---

## 📁 Project Files

| File | Purpose |
|------|---------|
| `main.c` | Main program logic and entry point |
| `users.dat` | Stores user credentials and profiles |
| `leave_requests.dat` | Contains all leave request records |
| `pending_users.dat` | Holds pending user registration approvals |
| `monthly_stats.png` | Sample monthly statistics visualization |
| `yearly_stats.png` | Sample yearly statistics visualization |

---

## 🚀 Getting Started

### Prerequisites
- GCC or any C compiler
- Terminal with ANSI color support

### Installation
```bash
# Clone the repository
git clone https://github.com/Zinoh0A/Leave-Request-Management-System-LRMS-.git
cd Leave-Request-Management-System-LRMS-

# Compile the program
gcc main.c -o lrms

# Run the system
./lrms  # On Unix/Linux
# or
lrms.exe  # On Windows
```

---

## 📊 Data Storage

The system uses binary files for efficient data storage:

- **`users.dat`** – Encrypted user credentials and profiles
- **`leave_requests.dat`** – Complete leave request history
- **`pending_users.dat`** – Users awaiting manager approval

All files are automatically created upon first run.

---

## 🎯 Usage Workflow

1. **Registration Phase**
   - New employees sign up (requires manager approval)
   - Managers/HR are pre-registered in the system

2. **Authentication**
   - Users log in with credentials
   - Password recovery available via security questions

3. **Employee Operations**
   - Submit leave requests
   - Check request status
   - Update personal details
   - View leave history

4. **Manager Operations**
   - Approve/reject pending requests
   - Review new user registrations
   - Generate leave statistics
   - Manage user accounts

---

## 🔒 Security Features

- **Password Protection** – Input masking prevents shoulder surfing
- **Data Encryption** – Binary storage format adds security layer
- **Role-Based Access** – Clear separation between employee/manager/hr functions
- **Session Management** – Secure login/logout mechanisms

---

## 🖥️ Technical Details

- **Language**: Pure C (no external dependencies)
- **Libraries**: Standard C libraries only (`stdio.h`, `stdlib.h`, `string.h`, `time.h`)
- **Platform**: Cross-platform (Windows/Linux/macOS)
- **UI**: Terminal-based with ANSI color formatting
- **Storage**: Binary file system for data persistence

---

## 📈 Statistics & Reporting

The system generates:
- **Monthly Leave Trends** – Visualized in `monthly_stats.png`
- **Yearly Analytics** – Summarized in `yearly_stats.png`
- **Employee Reports** – Individual leave history and patterns
- **Department Analytics** – Team-wise leave consumption

---

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

---

## 📄 License

This project is available as open source under the terms of the MIT License.

---

## 👤 Author

**ZinohOA** – [GitHub Profile](https://github.com/Zinoh0A)

---

## 🔗 Links

- **Repository**: https://github.com/Zinoh0A/Leave-Request-Management-System-LRMS-.git
- **Issues**: [Report Bugs](https://github.com/Zinoh0A/Leave-Request-Management-System-LRMS-/issues)

---

## ⭐ Support

If you find this project useful, please give it a star on GitHub!

---

*Last Updated: February 2024*  
*Version: 1.0.0*

