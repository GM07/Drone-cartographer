module.exports = {
  root: true,
  env: {
    node: true,
  },
  plugins: ['import', 'unicorn'],
  extends: [
    'plugin:vue/essential',
    'eslint:recommended',
    '@vue/typescript/recommended',
    '@vue/prettier',
    '@vue/prettier/@typescript-eslint',
    'plugin:import/recommended',
    'plugin:import/typescript',
  ],
  parserOptions: {
    ecmaVersion: 2020,
  },
  rules: {
    'import/no-unresolved': 'error',
    'import/no-default-export': 'error',
    'import/no-relative-packages': 'warn',
    'import/no-named-as-default-member': 'off',
    'unicorn/filename-case': [
      'error',
      {
        case: 'snakeCase',
      },
    ],
    'no-console': process.env.NODE_ENV === 'production' ? 'warn' : 'off',
    'no-debugger': process.env.NODE_ENV === 'production' ? 'warn' : 'off',
    'prettier/prettier': [
      'warn',
      {
        bracketSpacing: false,
        singleQuote: true,
        trailingComma: 'es5',
        arrowParens: 'avoid',
      },
    ],
    'block-scoped-var': 'error',
    eqeqeq: 'error',
    'no-var': 'error',
    'prefer-const': 'error',
    'eol-last': 'warn',
    'prefer-arrow-callback': 'error',
    'no-trailing-spaces': 'warn',
    quotes: ['warn', 'single', {avoidEscape: true}],
    'no-restricted-properties': [
      'error',
      {
        object: 'describe',
        property: 'only',
      },
      {
        object: 'it',
        property: 'only',
      },
    ],
    'vue/attributes-order': [
      'warn',
      {
        order: [
          'DEFINITION',
          'LIST_RENDERING',
          'CONDITIONALS',
          'RENDER_MODIFIERS',
          'GLOBAL',
          ['UNIQUE', 'SLOT'],
          'TWO_WAY_BINDING',
          'OTHER_DIRECTIVES',
          'OTHER_ATTR',
          'EVENTS',
          'CONTENT',
        ],
        alphabetical: true,
      },
    ],
    'vue/component-tags-order': [
      'warn',
      {
        order: [['script', 'template'], 'style'],
      },
    ],
    '@typescript-eslint/member-ordering': 'warn',
    '@typescript-eslint/naming-convention': [
      'error',
      {
        selector: 'variableLike',
        format: ['camelCase'],
      },
      {
        selector: 'variable',
        modifiers: ['const'],
        format: ['UPPER_CASE'],
      },
      {
        selector: 'classProperty',
        format: ['camelCase'],
      },
      {
        selector: 'classProperty',
        modifiers: ['readonly'],
        format: ['UPPER_CASE'],
      },
      {
        selector: 'method',
        format: ['camelCase'],
      },
      {
        selector: 'typeLike',
        format: ['PascalCase'],
      },
    ],
  },
  settings: {
    'import/parsers': {
      '@typescript-eslint/parser': ['.ts', '.tsx'],
    },
    'import/resolver': {
      typescript: {
        alwaysTryTypes: true,
        project: 'tsconfig.json',
      },
    },
  },
  overrides: [
    {
      files: [
        '**/__tests__/*.{j,t}s?(x)',
        '**/tests/unit/**/*.spec.{j,t}s?(x)',
      ],
      parser: 'vue-eslint-parser',
      extends: ['plugin:@typescript-eslint/recommended'],
      rules: {
        '@typescript-eslint/member-ordering': 'warn',
        '@typescript-eslint/no-non-null-assertion': 'off',
        '@typescript-eslint/no-use-before-define': 'off',
        '@typescript-eslint/no-warning-comments': 'off',
        '@typescript-eslint/no-empty-function': 'off',
        '@typescript-eslint/no-var-requires': 'off',
        '@typescript-eslint/explicit-function-return-type': 'off',
        '@typescript-eslint/explicit-module-boundary-types': 'off',
        '@typescript-eslint/ban-types': 'off',
        '@typescript-eslint/camelcase': 'off',
        'node/no-missing-import': 'off',
        'node/no-empty-function': 'off',
        'node/no-unsupported-features/es-syntax': 'off',
        'node/no-missing-require': 'off',
        'node/shebang': 'off',
        'no-dupe-class-members': 'off',
        'require-atomic-updates': 'off',
      },
      parserOptions: {
        parser: '@typescript-eslint/parser',
        ecmaVersion: 2018,
        sourceType: 'module',
      },
      env: {
        jest: true,
      },
    },
    {
      files: ['*.vue', '*.d.ts', '**/plugins/**/*.ts'],
      rules: {
        'import/no-default-export': 'off',
      },
    },
    {
      files: ['*.d.ts', '**/plugins/**/*.ts'],
      rules: {
        'unicorn/filename-case': 'off',
      },
    },
  ],
};
