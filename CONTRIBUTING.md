# Contributing to RLST

## Table of contents

[Style guides](#style-guides)
  - [Git branch strategy](#git-branch-strategy)
  - [Git commit messages](#git-commit-messages)

## Style guides

### Git branch strategy

The Branch Strategy is based on [git-flow-lite](http://nvie.com/posts/a-successful-git-branching-model/).

- **develop**: points to the development of the next release, contains tested and reviewed code.
- **feature/_<name>_**: points to a branch with a new feature, one which is candidate for merge into develop (subject to rebase).
- **fix/_<name>_**: points to a branch with a fix for a particular issue ID.

**Note:** The **latest** tag is used to point to the most recent stable release.

### Git commit messages

- Use the present tense ("add feature" not "added feature").
- Use the imperative mood ("Move cursor to..." not "Moves cursor to...").
- Limit the first line to 80 characters or less.
- Reference issues and pull requests liberally.
- If your pull request fixes an existing issue, add "Fixes: _<ISSUEURL>_" to your pull request description.
