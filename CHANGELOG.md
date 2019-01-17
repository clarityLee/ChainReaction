# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## 0.0.3 = 2018-12-22
### Added
- Add Evo2.cpp as final candidate to put on the platform.
### Changed
- Eliminate the use of share_ptr by reference. Elimiate unnecessary parameters.
- Adjust condition for gameover situation.

## 0.0.2 - 2018-12-21
### Added
- Add a local platform for the Chain Reaction algorithm for testing.
- Add Alpha-Beta Pruning to the Minimax algorithm. GameTree.hpp and GameTree.cpp are formal version for AB-Minimax. Smarter.cpp is the version for cloud platform.
- Add a Random algorithm as an opponent to AB-minimax algorithm.

## 0.0.1 - 2018-12-20
### Added
- A simple strategy using minimax algorithm.