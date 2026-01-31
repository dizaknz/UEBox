# AGENTS.md

---
domain: testing
tasks: [write-tests]
---

# Testing Guidelines

* write tests for Unreal Engine version 5.7
* when I ask for a unit test refer to https://dev.epicgames.com/documentation/en-us/unreal-engine/write-cplusplus-tests-in-unreal-engine
* when I ask for a spec test refer to https://dev.epicgames.com/documentation/en-us/unreal-engine/automation-spec-in-unreal-engine


## Unit tests

* for simple unit test use `IMPLEMENT_SIMPLE_AUTOMATION_TEST`
  * implement `RunTest`
* for complex unit test use `IMPLEMENT_COMPLEX_AUTOMATION_TEST`
  * implement `GetTests`
  * implement `RunTest`
* for tests that needs to run across multiple frames or async use `DEFINE_LATENT_AUTOMATION_COMMAND`
  * implement `Update`
  * register latent tests with `ADD_LATENT_AUTOMATION_COMMAND`


## Spec tests

* for simple spec tests with no state use `DEFINE_SPEC`
* for spec tests with state use `BEGIN_DEFINE_SPEC` and `END_DEFINE_SPEC`
* use `Describe` to describe a set of expectations to test
    * implement `BeforeEach` to run setup before each `It`
    * implement `AfterEach` to clean up after every `It`
    * implement `It` to define the behavarioul test
      * name the expectation using verbs, eg. `can open a map` or `should fail to open invalid map`

Example spec test with state

```
BEGIN_DEFINE_SPEC(MyCustomSpec, "MyGame.MyCustomClass", EAutomationTestFlags::ProductFilter | EAutomationTestFlags_ApplicationContextMask)
    TSharedPtr<FMyCustomClass> CustomClass;
END_DEFINE_SPEC(MyCustomSpec)
void MyCustomSpec::Define()
{
    Describe("Execute()", [this]()
    {
        It("should return true when successful", [this]()
        {
            TestTrue("Execute", CustomClass->Execute());
        });

        It("should return false when unsuccessful", [this]()
        {
            TestFalse("Execute", CustomClass->Execute());
        });
    });
}
```
