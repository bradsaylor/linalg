Perfect. Below is a **clean, consolidated procedure / conventions document** reflecting *exactly* what we agreed on. This is written so you can lift it directly into a repo doc (or split it across files later).

---

# Project Procedures and Conventions

This document defines the **contracts, documentation standards, testing procedures, error handling, and ownership rules** for this project. Its purpose is to remove ambiguity, prevent paradigm drift, and support systematic testing.

---

## 1. Documentation Structure

### 1.1 Public API documentation (`.h` files)

#### Per public function

Each **public API function** must be documented with a Doxygen-style comment immediately preceding its declaration. At minimum, the comment must include:

* `@brief`
* `@param` (for each parameter)
* `@return` (enumerated return codes and meanings)
* `@pre` (numbered preconditions)
* `@post` (numbered postconditions)
* `@ownership` (using standard ownership vocabulary)
* `@sideeffects` (if any)

Contract items in `@pre`, `@post`, and `@return` **must be numbered** (IDs), so they can be referenced directly by tests.

#### Per module / header

Each public header (`*.h`) must include a top-of-file comment describing:

* the purpose of the module
* module-level invariants and conventions
* any constraints shared by all functions in the API

#### Project-wide conventions

The project maintains a global conventions section describing:

* error code semantics
* ownership vocabulary
* assertion policy
* global invariants (e.g. NSE-CE)

These conventions apply unless a function explicitly documents an exception.

---

## 2. Commenting Policy for `.c` Files

### 2.1 Public function implementations

Public function **definitions** in `.c` files do **not** require full documentation blocks.

* The authoritative contract lives in the `.h`.
* Optional short comments may be used for:

  * non-obvious implementation details
  * algorithmic rationale
  * performance or safety notes that do *not* affect the external contract

**Rule:**
If it affects what callers may rely on, it belongs in the `.h`.
If it explains *how* something is done internally, it belongs in the `.c`.

### 2.2 Private / internal functions

All private (`static`) helper functions must be commented.

Minimum content:

* purpose (why the function exists)
* input assumptions
* effects (mutations, allocations, frees, refcount changes)
* return meaning (if non-obvious)

Design-decision notes should be included only when the code is non-obvious or enforces an invariant.

### 2.3 `.c` file–level comment

Each `.c` file should begin with a short comment describing:

* what the translation unit implements
* which module-level invariants it assumes
* any internal-only conventions used by helpers

---

## 3. Contracts and Invariants

### 3.1 Contract granularity

* **One contract per public function**
* Plus shared contracts at:

  * project level
  * module level

Paired functions (e.g. create/destroy, add/remove) are described via module invariants, not special “pair contracts.”

---

## 4. Global Invariant: No Side Effects on Caller Error

### 4.1 Name

**NSE-CE — No Side Effects on Caller Error**

### 4.2 Definition

If a public API returns a **caller-error** code (violated `@pre` conditions):

1. No externally-visible state is modified
2. No ownership changes occur (no free/incref/decref)
3. No partial persistence occurs (no half-inserted or reachable objects)

### 4.3 Documentation

Each public function must reference this invariant, e.g.:

* `@post (caller-error): NSE-CE applies (no side effects on precondition failure).`

### 4.4 Testing implication

At least one test per public function must validate NSE-CE behavior by:

* triggering a precondition failure
* asserting return code
* asserting unchanged state / ownership

---

## 5. Error Code Conventions

All public APIs returning `int` use the following semantics:

| Code | Meaning                                              |
| ---: | ---------------------------------------------------- |
|    0 | Success                                              |
|    1 | Caller error (violated `@pre`; NSE-CE applies)       |
|    2 | Allocation / resource failure (must fully roll back) |
|    3 | Internal invariant / internal API failure            |
|   >3 | Function-specific, documented errors                 |

### 5.1 Internal error (`3`) policy

* All code paths returning `3` must trigger an `assert()` in debug builds.
* In release builds, the assert is compiled out and the function returns `3`.
* Functions that can return `3` must document it explicitly.

---

## 6. Ownership Vocabulary

All ownership semantics must be described using the following standard terms:

### BORROW

* Function uses pointer during the call only
* Caller retains ownership
* No free, no store, no refcount change

### TAKE

* Ownership transfers to callee on success
* Caller must not free/use as owner after success
* On caller-error (`1`) or alloc-error (`2`), ownership reverts to caller

### RETAIN

* Function stores pointer beyond the call
* Caller retains ownership
* For refcounted objects: incref exactly once on success

### RELEASE

* Function releases a previously retained/taken object
* For refcounted objects: decref exactly once (may destroy)

### RETURN-NEW

* Returned pointer is newly owned by the caller
* Caller must eventually free/destroy/release it

If returning a borrowed pointer instead, it must be explicitly documented as **RETURN-BORROWED**.

---

## 7. Testing Policy

### 7.1 Source of truth

* Tests are derived directly from the numbered contract items in `.h` files.

### 7.2 Coverage rule

* **One test per contract item** (`PRE-*`, `POST-*`, `RET-*`, etc.).
* Tests must reference the contract ID directly in their name.

Example:

```
test_remove_binding_POST_CE1_no_sideeffects_on_caller_error()
```

### 7.3 Organization

* **One test file per module**, mirroring public headers:

  * `test_reg_hash.c`
  * `test_math_objs.c`
* Within each file, tests are grouped by public API function.

### 7.4 Fixtures / helpers

* Use `static` helper functions inside test files to handle setup/teardown.
* Helpers must not hide or assert the behavior under test.
* Each test must still explicitly assert the contract item it targets.

### 7.5 Adding a new public API function

Required:

1. Contract items exist and are numbered in the `.h`
2. Tests exist referencing each contract ID

Strongly recommended:

* At least one happy-path test
* At least one NSE-CE test
* ASAN run clean for the module tests

---

## 8. Change Control (Implicit Rule)

* Once a convention is adopted here, it is treated as **frozen**.
* Improvements are captured separately and applied deliberately, not mid-feature.
* Contract changes require updating documentation and tests alongside code.

---

This gives you a **stable operating system** for the project:

* contracts are explicit
* tests are mechanically derived
* ownership is unambiguous
* error handling is consistent
* creep is contained

Take your shot at implementing this where it belongs, and when you’re ready, we can sanity-check it against a real API or trim anything that feels heavier than it needs to be.
