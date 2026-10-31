# Tests

## Test types

Violet has E2E tests, integration tests, and unit-ish tests.

The E2E tests run all the way from the CLI, integration tests run on a full site, and unit-ish tests run on individual parts.

With the exception of unit-ish tests (which run on small units with input defined in the test code), the decision between E2E and integration test is whatever is most convenient for the bit. The E2E tests don't _need_ to be as big as the integration tests, since they're functionally identical. The E2E test just additionally allow testing parts of the CLI.

## Magic strings

### Debug markers

In certain files in the `sites` test data file tree, you will encounter debug markers. These are text in this format:
```markdown
\[\[violet-test(page):index\]\]
```

These are used by tests to extract content from the page in a relatively reliable way (regex). The raw rendered string must be in the format
```
[[violet-test(key unique to the page):content on a single line here]]
# Or with optional | used to escape content further. This can be any number, but must be matched before and after
[[violet-test(key unique to the page):||content on a single line here||]]
```

## Available fixtures

The following fixtures can be used directly (note that `tests::` is omitted from all of them because I can):

* `E2EFixture` - used for E2E tests
* `TestSiteFixture` - used for integration tests

In addition, the following utils are available:

* `WithTimezone`: Used to set `Config::timezone` to a specified variable, and auto-revert it after the test ends
* `IrisArgs`: Used for creating arguments to directly pass to an iris function. Useful for more direct API testing of specific functions directly. It's often more recommended to use the monolith test site with debug markers (see the magic strings section)
