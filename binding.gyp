{
  "targets": [
    {
      "target_name": "bartlby",
      "sources": [ "bartlby.cpp" ],
      'include_dirs': [
	"<!(node -p -e \"require('path').relative('.', require('path').dirname(require.resolve('nan')))\")",
      ]
    }
  ]
}
