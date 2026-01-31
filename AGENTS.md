# AGENTS.md
This document provides conventions and instructions for an AI agent assisting with Unreal Engine C++ project development. 

## Project Context

### Unreal Engine Version
- Target: Unreal Engine 5.3
- C++ Standard: C++17
- Platform Support: Windows

### Project Structure
- Follow established directory structure and naming conventions
- Understand module organization and build configuration
- Respect existing codebase patterns and architectural decisions

### Setup and run commands
- Run commands in a windows prompt `cmd`
- Initial setup: `GenerateProjectFiles.bat C:\Program Files\Epic Games\UE_5.3`
- Build command: `Build.bat`

### Test commands
- Run tests in a windows prompt `cmd`
- Run tests: `Test.bat`
[More](./Docs/TESTING.md)

### Code style
- Generate clean, well-formatted C++ code following Unreal Engine coding standards
- Use proper Unreal Engine patterns (UCLASS, UPROPERTY, UObject hierarchy)
- Implement proper error handling and null checks
[More](./Docs/CONVENTIONS.md)

## Agent Behavior Principles

### What to Avoid
- Do not generate code that violates Unreal Engine licensing
- Avoid creating circular dependencies in generated code
- Do not modify existing files without explicit instruction
- Refrain from generating unsafe code patterns

### When to Seek Human Input
- Complex architectural decisions should be discussed with human developers
- Game design and balance decisions are outside agent scope
- Major breaking changes should be reviewed by team leads
- Performance-critical sections may need manual optimization
 
### Consistency
- Maintain consistent code style throughout all generated code
- Follow established project conventions strictly
- Ensure compatibility with existing codebase patterns

### Quality Focus
- Prioritize correctness over speed in code generation
- Generate maintainable, readable code
- Include proper error handling and safety measures

### Learning and Adaptation
- Learn from project-specific patterns and requirements
- Adapt to team coding styles and preferences
- Improve based on feedback and code reviews

This agent should act as a capable assistant that generates high-quality Unreal Engine C++ code while maintaining consistency with established project standards and practices.
