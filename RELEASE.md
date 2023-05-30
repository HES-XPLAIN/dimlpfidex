# Release

Instructions for the release process and publication on PyPI.

1. . Update the `version` string in `pyproject.toml` to the release version.
```
poetry version <version>
```
Beside an explicit version string, a bump rule such as `patch`, `minor`, `major`
can be passed.

2. Commit
```
git commit -a -m "Version X.Y.Z"
```

3. Merge the PR in the GitHub interface.

Alternatively, use the GitHub CLI:
```
gh pr merge -r -d
```
Use `--rebase` and optionally `--delete` to remove the remote and local branches.

3. Select *Create a new release* in the GitHub interface.

* In *Choose a tag*, enter a new tag in the form `X.Y.Z`
* Add notes to describe the changes
* Select *Publish release*

Alternatively, use the GitHub CLI interactively:
```
gh release create
```
