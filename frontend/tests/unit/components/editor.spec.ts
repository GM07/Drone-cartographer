import {shallowMount, Wrapper} from '@vue/test-utils';
import {DefaultProps} from 'vue/types/options';
import * as SERVER_CONSTANTS from '@/communication/server_constants';
import Editor from '@/components/editor.vue';
import {ServerCommunication} from '@/communication/server_communication';
import {TreeNodeStub} from '../stubs/tree_node_stub';

describe('editor.vue', () => {
  let wrapper: Wrapper<DefaultProps & Editor, Element>;

  const STUBS = [
    'CodeEditor',
    'Treeselect',
    'RemoteCommandOutput',
    'vueCustomScrollbar',
    'VueResizable',
  ];

  beforeEach(() => {
    fetchMock.resetMocks();
    fetchMock.mockResponse(JSON.stringify(''), {
      status: SERVER_CONSTANTS.HTTP_OK,
    });
    spyOn(ServerCommunication, 'getFiles').and.returnValue(
      new Promise(resolve => {
        resolve({
          keys: ['', 'file.txt', '/folder/file.txt', '/folder/folder/file.txt'],
          values: ['content', 'content', 'content', 'content'],
        });
      })
    );

    wrapper = shallowMount<Editor>(Editor, {
      stubs: STUBS,
    });
  });

  it('should create', () => {
    expect(wrapper).toBeTruthy();
  });

  it('should clone a node', () => {
    expect(wrapper.vm.options[0].clone()).toEqual(wrapper.vm.options[0]);
  });

  it('should react on selected file', () => {
    const SPY = spyOn(wrapper.vm, 'changeFileContent');
    wrapper.vm.files = new Map();
    wrapper.vm.onFileSelected(new TreeNodeStub('a', 'a'));
    expect(SPY).not.toHaveBeenCalled();
    wrapper.vm.files.set('a', 'a');
    wrapper.vm.onFileSelected(new TreeNodeStub('a', 'a'));
    expect(SPY).toHaveBeenCalled();
  });

  it('should resize on move', () => {
    const SPY = spyOn(document, 'getElementById').and.returnValue({
      style: {
        width: 1,
      },
    });
    wrapper.vm.resizeMove({width: 0});
    expect(SPY).toHaveBeenCalled();
  });

  it('should save', () => {
    wrapper.vm.files.set('a', 'b');
    wrapper.vm.currentFileSelected = 'a';
    wrapper.vm.save('');
    expect(wrapper.vm.files.get('a')).toEqual('b');
    wrapper.vm.save('a');
    expect(wrapper.vm.files.get('a')).toEqual('a');
  });

  it('should recompile', async () => {
    const SEND_SPY = spyOn(ServerCommunication, 'sendFiles');
    const RECOMPILE_SPY = spyOn(ServerCommunication, 'recompile');
    await wrapper.vm.recompile();
    expect(SEND_SPY).toHaveBeenCalled();
    expect(RECOMPILE_SPY).toHaveBeenCalled();
  });

  it('should flash', async () => {
    const SEND_SPY = spyOn(ServerCommunication, 'sendFiles');
    const FLASH_SPY = spyOn(ServerCommunication, 'flash');
    await wrapper.vm.flash();
    expect(SEND_SPY).toHaveBeenCalled();
    expect(FLASH_SPY).toHaveBeenCalled();
  });

  it('should go back', () => {
    const SPY = spyOn(wrapper.vm, '$emit');
    wrapper.vm.goBack();
    expect(SPY).toHaveBeenCalled();
  });

  it('should change file content', () => {
    wrapper.vm.fileContent = 'a';
    wrapper.vm['changeFileContent']('b');
    expect(wrapper.vm.fileContent).toEqual('b');
  });
});
