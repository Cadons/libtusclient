# Contributing to <COMPANY>'s C++ Project 

Welcome! To ensure your contributions align with our workflow, please follow the steps below.

## Contribution Process

### 1. **Create an Issue**
   - Before making changes, **create an issue** in the GitLab repository.
   - Include a clear description, steps to reproduce (if it's a bug), expected behavior, and any relevant screenshots or logs
   - Add labels that characterize the issue type (e.g., bug, feature request, improvement, etc.).

### 2. **Fork the Repository**
   - If using a template like VPM, fork the repository to your GitLab account by clicking **Fork**.

### 3. **Clone Your Fork**
   Clone your fork to your local machine:
   ```bash
   git clone https://gitlab.com/your-username/your-forked-repo.git
   ```

### 4. **Create a New Branch**
   Create a branch based on `dev` for your feature or bug fix:
   ```bash
   git checkout -b 123-your-feature-name dev
   ```

### 5. **Make Changes**
   - Implement your changes according to the project's coding style and conventions.
   - Ensure any new features are fully tested.

### 6. **Write Tests**
   Write tests to validate your changes. To run tests locally with CMake and CTest:
   1. **Configure the project**:
      ```bash
      cmake --preset <Your generator>
      ```
   2. **Build the project**:
      ```bash
      cmake --build --preset <Your generator>
      ```
   3. **Run tests**:
      ```bash
      ctest
      ```
   - For detailed results:
     ```bash
     ctest --output-on-failure
     ```

### 7. **Commit Your Changes**
   Commit your changes with a clear message referencing the issue:
   ```bash
   git commit -m "Fix issue #123: Description of your fix"
   ```

### 8. **Push to Your Fork**
   Push your changes to your forked repository:
   ```bash
   git push origin 123-your-feature-name
   ```

### 9. **Create a Merge Request (MR)**
   - Create a **Merge Request (MR)** from your branch to `dev`.
   - In the MR description, reference the issue (e.g., `Fixes #123`), summarize the changes, and list any testing performed.

### 10. **Request Review**
   - Request a review from team members or maintainers. Ensure all tests have passed and the MR is ready for review.

### 11. **Revise and Finalize the MR**
   - If revisions are requested, update your branch, commit the changes, and push them.
   - Once approved, the MR will be merged into `dev`.

### 12. **Merge into `dev`**
   - After approval, your MR will be merged into `dev`. If unsure, consult a maintainer for guidance.

### 13. **Clean Up**
   - After merging, delete your feature branch setting delete branch in the MR settings

## Code Style Guidelines

- **Indentation**: 4 spaces per level (no tabs).
- **Naming**: 
   - Use camelCase for variables and functions.
   - Use PascalCase for classes.
- **Documentation**: Document complex code and functions.

## Reporting Bugs
   When reporting bugs, provide:
   - A clear description of the issue.
   - Steps to reproduce (if applicable).
   - Relevant screenshots, error logs, and environment details.

## Additional Resources

- [GitLab Workflow Documentation](https://docs.gitlab.com/ee/workflow/)
- [Creating a Merge Request in GitLab](https://docs.gitlab.com/ee/user/project/merge_requests/)
- [Git Workflow](https://www.atlassian.com/git/tutorials/comparing-workflows)
