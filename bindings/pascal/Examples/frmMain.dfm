object frMain: TfrMain
  Left = 248
  Top = 137
  Caption = 'frMain'
  ClientHeight = 394
  ClientWidth = 478
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = True
  Position = poDesktopCenter
  PixelsPerInch = 96
  TextHeight = 13
  object pcMain: TPageControl
    Left = 0
    Top = 0
    Width = 478
    Height = 394
    ActivePage = tsMessage
    Align = alClient
    TabOrder = 0
    object tsMessage: TTabSheet
      Caption = 'Message'
      object lbType: TLabel
        Left = 67
        Top = 20
        Width = 24
        Height = 13
        Caption = 'Type'
        Color = clBtnFace
        ParentColor = False
      end
      object lbFrom: TLabel
        Left = 65
        Top = 61
        Width = 24
        Height = 13
        Caption = 'From'
        Color = clBtnFace
        ParentColor = False
      end
      object lbTo: TLabel
        Left = 264
        Top = 61
        Width = 12
        Height = 13
        Caption = 'To'
        Color = clBtnFace
        ParentColor = False
      end
      object lbSubject: TLabel
        Left = 53
        Top = 100
        Width = 36
        Height = 13
        Caption = 'Subject'
        Color = clBtnFace
        ParentColor = False
      end
      object lbAttachments: TLabel
        Left = 23
        Top = 136
        Width = 61
        Height = 13
        Caption = 'Attachments'
        Color = clBtnFace
        ParentColor = False
      end
      object lbMessage: TLabel
        Left = 43
        Top = 192
        Width = 42
        Height = 13
        Caption = 'Message'
        Color = clBtnFace
        ParentColor = False
      end
      object cbType: TComboBox
        Left = 106
        Top = 16
        Width = 139
        Height = 21
        Style = csDropDownList
        ItemIndex = 0
        TabOrder = 0
        Text = 'Text'
        Items.Strings = (
          'Text'
          'HTML')
      end
      object edFrom: TEdit
        Left = 106
        Top = 56
        Width = 139
        Height = 21
        TabOrder = 2
        Text = 'user@gmail.com'
      end
      object edTo: TEdit
        Left = 288
        Top = 56
        Width = 139
        Height = 21
        TabOrder = 3
        Text = 'otheruser@gmail.com'
      end
      object edSubject: TEdit
        Left = 106
        Top = 96
        Width = 321
        Height = 21
        TabOrder = 4
        Text = 'Test'
      end
      object lsAttachments: TListBox
        Left = 106
        Top = 136
        Width = 321
        Height = 48
        ItemHeight = 13
        ScrollWidth = 319
        TabOrder = 5
        OnClick = lsAttachmentsClick
        OnDblClick = lsAttachmentsDblClick
      end
      object txMessage: TMemo
        Left = 106
        Top = 192
        Width = 321
        Height = 138
        Lines.Strings = (
          'Testing XMailer! :-)')
        TabOrder = 6
      end
      object btSend: TButton
        Left = 352
        Top = 16
        Width = 75
        Height = 25
        Caption = 'Send'
        TabOrder = 1
        OnClick = btSendClick
      end
    end
    object tsConfig: TTabSheet
      Caption = 'Config'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object lbHost: TLabel
        Left = 44
        Top = 54
        Width = 22
        Height = 13
        Caption = 'Host'
        Color = clBtnFace
        ParentColor = False
      end
      object lbPort: TLabel
        Left = 271
        Top = 54
        Width = 20
        Height = 13
        Caption = 'Port'
        Color = clBtnFace
        ParentColor = False
      end
      object lbUser: TLabel
        Left = 43
        Top = 101
        Width = 22
        Height = 13
        Caption = 'User'
        Color = clBtnFace
        ParentColor = False
      end
      object lbPwd: TLabel
        Left = 240
        Top = 101
        Width = 46
        Height = 13
        Caption = 'Password'
        Color = clBtnFace
        ParentColor = False
      end
      object lbTimeout: TLabel
        Left = 24
        Top = 149
        Width = 38
        Height = 13
        Caption = 'Timeout'
        Color = clBtnFace
        ParentColor = False
      end
      object edHost: TEdit
        Left = 80
        Top = 48
        Width = 136
        Height = 27
        TabOrder = 0
        Text = 'smtp.gmail.com'
      end
      object edUser: TEdit
        Left = 80
        Top = 96
        Width = 136
        Height = 27
        TabOrder = 2
        Text = 'user@gmail.com'
      end
      object edPort: TSpinEdit
        Left = 304
        Top = 48
        Width = 136
        Height = 27
        MaxValue = 65535
        MinValue = 1
        TabOrder = 1
        Value = 587
      end
      object edPwd: TEdit
        Left = 304
        Top = 96
        Width = 136
        Height = 27
        PasswordChar = '*'
        TabOrder = 3
      end
      object edTimeout: TSpinEdit
        Left = 80
        Top = 144
        Width = 136
        Height = 27
        MaxValue = 65535
        MinValue = 0
        TabOrder = 4
        Value = 3000
      end
      object chTLS: TCheckBox
        Left = 251
        Top = 146
        Width = 43
        Height = 21
        Caption = 'TLS'
        Checked = True
        State = cbChecked
        TabOrder = 5
      end
      object chVerifyCert: TCheckBox
        Left = 326
        Top = 146
        Width = 114
        Height = 21
        Caption = 'Verify certificate'
        TabOrder = 6
      end
    end
  end
  object XMailerSMTP1: TXMailerSMTP
    OnError = XMailerSMTP1Error
    Left = 112
    Top = 240
  end
  object XMailerMessage1: TXMailerMessage
    SMTP = XMailerSMTP1
    ContentType = xmctText
    Left = 224
    Top = 240
  end
  object OpenDialog1: TOpenDialog
    Left = 320
    Top = 240
  end
end
