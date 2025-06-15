# Advanced Tic Tac Toe Game

![CI/CD Pipeline](https://github.com/MohamedElnewehy/advanced-tictactoe-cpp/workflows/Advanced%20Tic%20Tac%20Toe%20CI/CD%20Pipeline/badge.svg)
![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)
![Qt Version](https://img.shields.io/badge/Qt-6.5.0+-green.svg)
![C++](https://img.shields.io/badge/C++-17-blue.svg)

## Overview

Advanced Tic Tac Toe game built with Qt6 and C++, featuring intelligent AI opponents, secure user authentication, and comprehensive game history tracking. This project demonstrates professional software engineering practices including object-oriented design, automated testing, and CI/CD integration.

## Features

### Core Game Mechanics
- **Multiple Game Modes**
  - Classic Mode: Traditional 3x3 Tic Tac Toe
  - Overwrite Mode: Advanced gameplay with strategic overwriting
  - AI Mode: Player vs Computer with multiple difficulty levels

### AI Implementation
- **Easy AI**: Random move selection
- **Medium AI**: 70% optimal moves, 30% random
- **Hard AI**: Minimax algorithm with alpha-beta pruning for optimal play

### User Management
- **Secure Authentication**: SHA-256 password hashing
- **User Registration**: Account creation with security questions
- **Session Management**: Persistent login sessions
- **Password Recovery**: Security question-based password reset

### Game History & Analytics
- **Complete Game Tracking**: All moves and outcomes recorded
- **Game Replay**: Visual replay of past games with speed controls
- **User Statistics**: Win/loss ratios and performance metrics
- **History Management**: Filter and search game history

### Professional GUI
- **Modern Qt6 Interface**: Clean, responsive design
- **Visual Move Display**: Immediate feedback with 500ms delay before result messages
- **Smooth Animations**: Enhanced user experience
- **Cross-Platform Compatibility**: Windows, Linux, macOS

## Technical Architecture

### Design Patterns
- **Model-View-Controller (MVC)**: Clean separation of concerns
- **Strategy Pattern**: Interchangeable AI algorithms
- **Observer Pattern**: UI updates and game state management

### Data Structures
- **Trees**: Minimax algorithm implementation
- **Vectors**: Board state and move history storage
- **Hash Tables**: User session management
- **Stacks**: Game state tracking

### Database Schema
```sql
-- Users table with security features
users (id, username, password_hash, favorite_color, favorite_meal, 
       favorite_word, birth_place, created_at)

-- Game history tracking
games (id, player1_id, player2_id, winner, moves, game_mode, 
       game_duration, timestamp)

-- Session management
user_session (id, username, login_time)
