import {SERVER_ADDRESS} from '@/communication/server_constants';

/**
 * Tests variables used in testing environment.
 * Inspired from https://stackoverflow.com/questions/48033841/test-process-env-with-jest
 */
describe('server_constants.vue', () => {
  const OLD_ENV = process.env;

  afterAll(() => {
    process.env = OLD_ENV;
  });

  it('should not use a real socket path when testing', () => {
    expect(SERVER_ADDRESS).toEqual('');
  });

  it('should use a real socket path when not testing', () => {
    jest.resetModules();
    process.env = {...OLD_ENV};
    process.env.NODE_ENV = 'dev';
    const TESTED_SERVER_ADDRESS =
      require('@/communication/server_constants').SERVER_ADDRESS;

    expect(TESTED_SERVER_ADDRESS).not.toEqual('');
  });
});
